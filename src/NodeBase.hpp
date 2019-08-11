#pragma once
#include <vector> // for vector
#include "Elements.hpp"

namespace btree {
#define NODE_TEMPLATE template <typename Key, typename Value, uint16_t BtreeOrder>
#define BASE NodeBase<Key, Value, BtreeOrder>

	NODE_TEMPLATE
	class NodeBase {
	public:
		using Ele      = Elements<Key, Value, BtreeOrder, NodeBase>;
		using LessThan = typename Ele::LessThan;

		template <typename Iter>
		NodeBase(Iter, Iter, shared_ptr<LessThan>);
		NodeBase(const NodeBase&);
		NodeBase(NodeBase&&) noexcept;
		virtual unique_ptr<NodeBase> clone() const = 0;
		virtual ~NodeBase() = default;

		inline bool        middle() const;
		inline const Key&  maxKey() const;
		inline vector<Key> allKey() const;
		inline bool        have  (const Key&, vector<NodeBase*>&);
		inline bool        have  (const Key&) const;
		const Value*       search(const Key&) const;
		void               add   (pair<Key, Value>, vector<NodeBase*>&);
		bool               remove(const Key&);

	protected:
		Elements<Key, Value, BtreeOrder, NodeBase> elements_;

		uint16_t      childCount() const;
		inline bool   full()  const;
		inline bool   empty() const;
		inline void   changeInSearchDownPath(const Key&, const Key&);
		inline Value* searchWithSaveTrack(const Key&, vector<NodeBase*>&);

		// TODO add not only key-value add, but also key-unique_ptr add
		template <typename T>
		void doAdd(pair<Key, T>, vector<NodeBase*>&);
		template <typename T>
		inline bool tryPreviousAdd(pair<Key, T>&, vector<NodeBase*>&);
		template <typename T>
		inline bool tryNextAdd    (pair<Key, T>&, vector<NodeBase*>&);
		template <typename T>
		void        splitNode(pair<Key, T>, vector<NodeBase*>&);

		template <typename T>
		bool reallocatePre(NodeBase *, vector<NodeBase *> &, pair<Key, T>);
		template <typename T>
		bool reallocateNxt(NodeBase *, pair<Key, T>);
		void changeMaxKeyUpper(const vector<NodeBase *> &, const Key&) const;
		void replacePreviousNodeMaxKeyInTreeBySearchUpIn(const vector<NodeBase *> &, NodeBase *, const Key &);
		void insertLeafToUpper(unique_ptr<NodeBase>, vector<NodeBase*>&);
		inline bool searchHelper(const Key &, function<void(NodeBase *)>,
		                                      function<bool(NodeBase *)>,
		                                      function<bool(NodeBase *)>);

		static bool spaceFreeIn(const NodeBase*);
	};
}

namespace btree {
	using std::make_pair;

	NODE_TEMPLATE
	template <typename Iter>
	BASE::NodeBase(Iter begin, Iter end, shared_ptr<LessThan> lessThanPtr)
		: elements_(begin, end, lessThanPtr)
	{ }

	NODE_TEMPLATE
	BASE::NodeBase(const NodeBase& that)
		: elements_(that.elements_)
	{ }

	NODE_TEMPLATE
	BASE::NodeBase(NodeBase&& that) noexcept
		: elements_(std::move(that.elements_))
	{ }

	NODE_TEMPLATE
	uint16_t
	BASE::childCount() const
	{
		return elements_.count();
	}

	NODE_TEMPLATE
	bool
	BASE::empty() const
	{
		return elements_.count() == 0;
	}	

	NODE_TEMPLATE
	bool
	BASE::full() const
	{
		return elements_.full();
	}

	NODE_TEMPLATE
	bool
	BASE::middle() const
	{
		return elements_.MiddleFlag;
	}

	NODE_TEMPLATE
	const Key&
	BASE::maxKey() const
	{
		return elements_[elements_.count() - 1].first;
	}

	NODE_TEMPLATE
	bool
	BASE::searchHelper(
		const Key &key,
		function<void(NodeBase *)> operateOnNode,
		function<bool(NodeBase *)> equalHandler,
		function<bool(NodeBase *)> beyondMaxHandler
	)
	{
		auto& lessThan = *(elements_.LessThanPtr);

		function<bool(NodeBase*)> helper = [&] (NodeBase* node) {
			operateOnNode(node); // record node or other operation

			for (auto& e : elements_) {
				if (lessThan(key, e.first)) {
					if (middle()) {
						return helper(Ele::ptr(e.second));
					} else {
						return false;
					}
				} else if (!lessThan(e.first, key)) {
					return equalHandler(this);
				}
			}

			return beyondMaxHandler(this);
		};

		return helper(this);
	}

	NODE_TEMPLATE
	bool
	BASE::have(const Key& key, vector<NodeBase*>& passedNodeTrackStack)
	{
		auto& stack = passedNodeTrackStack;

		// only need to collect "don't have" situation
		auto collect = [&] (NodeBase* node) {
			stack.push_back(node);
		};
		function<bool(NodeBase*)> collectDeepMaxOnBeyond = [&] (NodeBase* node) {
			if (node->middle()) {
				auto maxIndex = node->childCount() - 1;
				auto maxChildPtr = Ele::ptr(node->elements_[maxIndex].second);
				collect(maxChildPtr);

				return collectDeepMaxOnBeyond(maxChildPtr);
			}

			return false;
		};
		auto trueOnEqual = [] (auto) { return true; };
		
		return searchHelper(key, collect, trueOnEqual, collectDeepMaxOnBeyond);
	}

	NODE_TEMPLATE
	bool
	BASE::have(const Key& key) const
	{
		auto doNothing     = [] (auto) { };
		auto trueOnEqual   = [] (auto) { return true; };
		auto falseOnBeyond = [] (auto) { return false; };

		return const_cast<NodeBase *>(this)->searchHelper(key, doNothing, trueOnEqual, falseOnBeyond);
	}

	NODE_TEMPLATE
	const Value*
	BASE::search(const Key& key) const
	{
		NodeBase* deepestNode;
		auto keepDeepest = [&] (NodeBase* node) {
			deepestNode = node;
		};
		auto falseOnBeyond = [] (auto) { return false; };
		function<bool(NodeBase*)> moveDeepOnEqual = [&] (NodeBase* node) {
			if (node->middle()) {
				auto maxIndex = node->childCount() - 1;
				auto maxChildPtr = Ele::ptr(node->elements_[maxIndex].second);
				keepDeepest(maxChildPtr);

				return moveDeepOnEqual(maxChildPtr);
			}

			return true;
		};

		if (const_cast<NodeBase *>(this)->
			searchHelper(key, keepDeepest, moveDeepOnEqual, falseOnBeyond)) {
			return &Ele::value_Ref(deepestNode->elements_[key]);
		}

		return nullptr;
	}

	NODE_TEMPLATE
	vector<Key>
	BASE::allKey() const
	{
		vector<Key> keys;
		keys.reserve(elements_.count());
		for (auto& e : elements_) {
			keys.emplace_back(e.first);
		}

		return keys;
	}

	NODE_TEMPLATE
	void
	BASE::add(pair<Key, Value> p, vector<NodeBase*>& passedNodeTrackStack)
	{
		auto& stack = passedNodeTrackStack;
		auto finalLeaf = stack.back();

		finalLeaf->doAdd(std::move(p), stack);
	}


	NODE_TEMPLATE
	template<typename T>
	void
	BASE::doAdd(pair<Key, T> p, vector<NodeBase*>& passedNodeTrackStack)
	{
		// TODO how to keep w/2 to w
		auto& k = p.first;
		auto& stack = passedNodeTrackStack;
		auto& lessThan = *(elements_.LessThanPtr);

		if (!full()) {
			if (lessThan(k, maxKey())) {
				elements_.insert(std::move(p));
			} else {
				elements_.append(std::move(p));
				changeMaxKeyUpper(stack, k);
			}
		} else {
			if (tryPreviousAdd<Key, Value, BtreeOrder, T>(p, stack)) {

			} else if (tryNextAdd<Key, Value, BtreeOrder, T>(p, stack)) {

			} else {
				splitNode(std::move(p), stack);
			}
		}
	}

	NODE_TEMPLATE
	bool
	BASE::remove(const Key& key)
	{
		auto maxValue = [] (Ele& e) -> typename Ele::ValueForContent& {
			return e[e.count() - 1].second;
		};

		// TODO wrong
		// Keep internal node key count between w/2 and w
		if (!middle()) {
			elements_.remove(key);
			// check count
		} else {
			for (auto& e : elements_) {
				if ((*elements_.LessThanPtr)(key, e.first)) {
					// Recursive locate
				} else if (!(*elements_.LessThanPtr)(e.first, key)) {
					auto subNodePtr = Ele::ptr(e.second);

				SearchInThisNode:
					if (!subNodePtr->middle()) {
						subNodePtr->elements_.remove(key);
						// Check count
					} else {
						subNodePtr = Ele::ptr(maxValue(subNodePtr->elements_));
						goto SearchInThisNode;
					}
				}
			}
		}

		return false; // TODO wait to modify
	}


	NODE_TEMPLATE
	void
	BASE::changeInSearchDownPath(const Key& oldKey, const Key& newKey)
	{
		vector<NodeBase*> trackStack{};

		searchWithSaveTrack(oldKey, trackStack);

		auto rend = trackStack.rend();
		for (auto rIter = trackStack.rbegin(); rIter != rend; ++rIter) {
			auto& node = *rIter;

			auto i = node->elements_.indexOf(oldKey);
			if (i != -1 && node->middle()) { // No need to change leaf
				node->elements_[i].first = newKey;
			}
		}

	}

	// duplicate maybe
	NODE_TEMPLATE
	Value*
	BASE::searchWithSaveTrack(const Key& key, vector<NodeBase*>& trackStack)
	{
		// TODO modify, or think of search
		// TODO maybe not need return value
		trackStack.push_back(this);

		auto maxValueForContent = [] (Ele& e) -> typename Ele::ValueForContent& {
			return e[e.count() - 1].second;
		};

		if (!middle()) {
			return elements_.have(key) ? &Ele::value_Ref(elements_[key]) : nullptr;
		} else {
			for (auto& e : elements_) {
				if ((*elements_.LessThanPtr)(key, e.first)) {
					return Ele::ptr(e.second)->searchWithSaveTrack(key, trackStack);
				} else if (!(*elements_.LessThanPtr)(e.first, key)) {
					auto subNodePtr = Ele::ptr(e.second);

				SearchInThisNode:
					trackStack.push_back(subNodePtr);

					if (!subNodePtr->middle()) {
						return &Ele::value_Ref(maxValueForContent(subNodePtr->elements_));
					} else {
						subNodePtr = Ele::ptr(maxValueForContent(subNodePtr->elements_));
						goto SearchInThisNode;
					}
				}
			}

			return nullptr;
		}
	}

	NODE_TEMPLATE
	template <typename T>
	bool
	BASE::reallocateNxt(NodeBase *nextNode, pair<Key, T> insertPair)
	{
		nextNode->elements_.insert(std::move(insertPair));

		return true;
	}

	NODE_TEMPLATE
	template <typename T>
	bool
	BASE::reallocatePre(NodeBase *previousNode, vector<NodeBase *> &passedNodeTrackStack, pair<Key, T> appendPair)
	{
		auto& stack = passedNodeTrackStack;

		previousNode->elements_.append(std::move(appendPair));
		auto newMaxKey = previousNode->maxKey(); // will change previous max
		// TODO could use this stack to get sibling stack, then change upper
		replacePreviousNodeMaxKeyInTreeBySearchUpIn(stack, previousNode, newMaxKey);

		return true;
	}

	/**
	 * Key type should be copyable
	 */
	NODE_TEMPLATE
	void
	BASE::changeMaxKeyUpper(const vector<NodeBase *> &passedNodeTrackStack, const Key& newMaxKey) const
	{
		auto& stack = passedNodeTrackStack;
		auto rCurrentIter = stack.rbegin();
		auto rEnd  = stack.rend();

		if ((rEnd - rCurrentIter) == 1) {
			return;
		}

		while (rCurrentIter != rEnd) {
			auto upperNodeIter = rCurrentIter + 1;
			auto matchIndex = upperNodeIter->elements_.changeKeyOf(*rCurrentIter, newMaxKey);

			auto maxIndex = upperNodeIter->childCount() - 1;
			if (matchIndex != maxIndex) {
				break;
			}

			++rCurrentIter;
		}
	}

	// use stack to get root node(maybe not need root node), then use this early max key to search
	NODE_TEMPLATE
	void
	BASE::replacePreviousNodeMaxKeyInTreeBySearchUpIn(
		const vector<NodeBase *> &passedNodeTrackStack,
		NodeBase *previousNode,
		const Key &newKey)
	{
		auto& stack = passedNodeTrackStack;
		// use iterator
		auto emitUpperNode = [&] {
			auto last = stack.back();
			stack.pop_back();

			return last;
		};

		// TODO sometimes it's not leaf
		emitUpperNode(); // top pointer is leaf, it's useless
		

		for (NodeBase* node = emitUpperNode(); stack.size() != 0; node = emitUpperNode()) {
			// judge if the node is the same ancestor between this and previous node
			if (node->have(previousNode)) {
				node->changeInSearchDownPath(previousNode, newKey);

				// judge if need to change upper node
				auto i = node->elements_.indexOf(previousNode); // should create a method called this layer search
				auto maxIndex = childCount() - 1;
				if (i != -1 && i == maxIndex) {
					stack.push_back(node);
					// change upper node
					changeMaxKeyUpper(stack, newKey);
					break;
				}
			}
		}
	}

	template <typename Key, typename Value, uint16_t BtreeOrder, typename T>
	void
	setSiblings(BASE*, BASE*);

	NODE_TEMPLATE
	template <typename T>
	void
	BASE::splitNode(pair<Key, T> p, vector<NodeBase*>& passedNodeTrackStack)
	{
		auto& key = p.first;
		auto& lessThan = *(elements_.LessThanPtr);
		auto& stack = passedNodeTrackStack;

		auto newPre = this->clone(); // TODO have a problem
		auto newPrePtr = newPre.get();
		// left is newPre, right is this
		setSiblings<Key, Value, BtreeOrder, T>(newPrePtr, this);

		auto i = elements_.suitablePosition(key);

		// [] not need reference if don't have last sentence?
		auto moveItems = [&] (uint16_t preNodeRemoveCount) {
			newPrePtr->elements_.removeItems(false, preNodeRemoveCount);
			this->elements_.removeItems(true, BtreeOrder - preNodeRemoveCount);
		};

		auto addIn = [&] (NodeBase* node, bool shouldAppend) {
			if (shouldAppend) {
				node->elements_.append(std::move(p));
			} else {
				node->elements_.insert(std::move(p));
			}
		};

#define HANDLE_ADD(leaf, maxBound) auto shouldAppend = (i == maxBound); do { addIn(leaf, shouldAppend); } while(0)

		constexpr bool odd = BtreeOrder % 2;
		constexpr auto middle = odd ? (BtreeOrder / 2 + 1) : (BtreeOrder / 2);
		if (i <= middle) {
			constexpr auto removeCount = middle;
			moveItems(removeCount);

			HANDLE_ADD(newPrePtr, middle);
		} else {
			constexpr auto removeCount = BtreeOrder - middle;
			moveItems(removeCount);

			HANDLE_ADD(this, BtreeOrder);
			if (shouldAppend) {
				auto stackCopy = stack;
				changeMaxKeyUpper(stackCopy, key);
			}
		}
		
#undef HANDLE_ADD

		insertLeafToUpper(std::move(newPre), stack);
	}

	NODE_TEMPLATE
	void
	BASE::insertLeafToUpper(unique_ptr<NodeBase> node, vector<NodeBase*>& passedNodeTrackStack)
	{
#define EMIT_UPPER_NODE() stack.pop_back()

		// reduce useless node, prepare to upper level add, like start new leaf add
		auto& stack = passedNodeTrackStack;
		EMIT_UPPER_NODE();
		
		auto pair = make_pair<Key, unique_ptr<NodeBase>>(node->maxKey(), std::move(node));
		doAdd(std::move(pair), stack);
		// 这部分甚至可能动 root，做好心理准备

#undef EMIT_UPPER_NODE
	}

	NODE_TEMPLATE
	bool
	BASE::spaceFreeIn(const NodeBase* node)
	{
		if (node != nullptr) {
			return !node->full();
		}

		return false;
	}

	template <typename Key, typename Value, uint16_t BtreeOrder, typename T>
	void
	getPrevious(BASE*, vector<BASE*>, BASE*&);
	/**
	 * @return means succeed or not
	 */
	NODE_TEMPLATE
	template <typename T>
	bool
	BASE::tryPreviousAdd(pair<Key, T>& p, vector<NodeBase *> &passedNodeTrackStack)
	{
		auto& stack = passedNodeTrackStack;
		NodeBase *previous = nullptr;

		getPrevious<Key, Value, BtreeOrder, T>(this, stack, previous); // some don't have one of siblings

		if (spaceFreeIn(previous)) {
			// if not free, will not trigger move, so the type is ref
			auto maxChanged = false;
			auto&& min = elements_.exchangeMin(std::move(p), maxChanged);
			if (maxChanged) {
				changeMaxKeyUpper(stack, maxKey());
			}

			return reallocatePre(previous, stack, std::move(min)); // trigger optimize
		}

		return false;
	}

	template <typename Key, typename Value, uint16_t BtreeOrder, typename T>
	void
	getNext(BASE*, vector<BASE*>, BASE*&);
	/**
	 * @return means succeed or not
	 */
	NODE_TEMPLATE
	template <typename T>
	bool
	BASE::tryNextAdd(pair<Key, T>& p, vector<NodeBase*>& passedNodeTrackStack)
	{
		auto& stack = passedNodeTrackStack;
		NodeBase *next = nullptr;

		getNext<Key, Value, BtreeOrder, T>(this, stack, next); // some don't have one of siblings

		if (spaceFreeIn(next)) {
			// if not free, will not trigger move, so the type is ref
			auto&& oldMax = elements_.exchangeMax(std::move(p));
			changeMaxKeyUpper(stack, maxKey());
			return reallocateNxt(next, std::move(oldMax));
		}

		return false;
	}

#undef BASE
#undef NODE_TEMPLATE
}
