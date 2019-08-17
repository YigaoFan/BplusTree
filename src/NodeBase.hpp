#pragma once
#include <vector> // for vector
#include "Util.hpp"
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
		virtual unique_ptr<NodeBase> move () const = 0;
		virtual ~NodeBase() = default;

		inline bool        middle() const;
		inline const Key&  maxKey() const;
		inline vector<Key> allKey() const;
		inline bool        have  (const Key&, vector<NodeBase*>&);
		inline bool        have  (const Key&) const;
		Value              search(const Key &) const;
		void               modify(const Key &, Value);
		void               add   (pair<Key, Value>, vector<NodeBase*>&);
		void               remove(const Key&, vector<NodeBase*>&);

	protected:
		Elements<Key, Value, BtreeOrder, NodeBase> elements_;

		inline uint16_t childCount() const;
		inline bool     full()  const;
		inline bool     empty() const;

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
		void insertNewPreToUpper(unique_ptr<NodeBase>, vector<NodeBase*>&);
		inline bool searchHelper(const Key &, function<void(NodeBase *)>,
		                                      function<bool(NodeBase *)>,
		                                      function<bool(NodeBase *)>);

		template <typename T>
		inline void doRemove(const T&, vector<NodeBase*>&);
		template <bool IS_LEAF>
		bool reBalance(const vector<NodeBase*>&);
		bool chooseBalanceNodeDo(NodeBase *, function<bool(NodeBase*)>,
								 NodeBase *, function<bool()>,
								 NodeBase *, function<bool(NodeBase*)>) const;
		template <bool IS_LEAF>
		bool reBalanceWithPre(NodeBase*, const vector<NodeBase*>&);
		template <bool IS_LEAF>
		bool reBalanceWithNxt(NodeBase*, const vector<NodeBase*>&);
		void receive(TailAppendWay, NodeBase&&);
		void receive(HeadInsertWay, NodeBase&&);
		void receive(HeadInsertWay, uint16_t, NodeBase&);
		void receive(TailAppendWay, uint16_t, NodeBase&);

		static bool spaceFreeIn(const NodeBase*);
		static vector<NodeBase*> getSiblingSearchTrackIn(const vector<NodeBase*>&, const NodeBase*);
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

		return const_cast<NodeBase *>(this)->searchHelper(key, std::move(doNothing),
			                                                   std::move(trueOnEqual),
			                                                   std::move(falseOnBeyond));
	}

	NODE_TEMPLATE
	void
	BASE::modify(const Key &key, Value value)
	{
		NodeBase* deepestNode = nullptr;
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

		if (searchHelper(key, keepDeepest, moveDeepOnEqual, falseOnBeyond)) {
			Ele::value_Ref(deepestNode->elements_[key]) = value;
		}
	}

	NODE_TEMPLATE
	Value
	BASE::search(const Key & key) const
	{
		NodeBase* deepestNode = nullptr;
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

		// TODO 注意检查 stack 是否收集到所需要的节点才停止
		if (const_cast<NodeBase*>(this)->searchHelper(key, keepDeepest, moveDeepOnEqual, falseOnBeyond)) {
			return Ele::value_Copy(deepestNode->elements_[key]);
		}

		throw runtime_error("don't have this key");
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
			// maybe fine the choose of pre and next
			if (tryPreviousAdd<T>(p, stack)) {

			} else if (tryNextAdd<T>(p, stack)) {

			} else {
				splitNode(std::move(p), stack);
			}
		}
	}

	NODE_TEMPLATE
	void
	BASE::remove(const Key& key, vector<NodeBase*>& passedNodeTrackStack)
	{
		auto& stack = passedNodeTrackStack;
		NodeBase* finalLeaf = stack.back();
		
		finalLeaf->doRemove(key, stack);
	}

	/**
	 * @tparam T type Key or PtrType*(NodeBase*)
	 */
	NODE_TEMPLATE
	template <typename T>
	void
	BASE::doRemove(const T& t, vector<NodeBase*>& passedNodeTrackStack)
	{
		auto& stack = passedNodeTrackStack;

		auto i = elements_.indexOf(t);
		if (elements_.remove(i)) {
			changeMaxKeyUpper(stack, maxKey());
		}

		// TODO is_same need to verify
		auto combined = reBalance<std::is_same<T, Key>::value>(stack);
		if (combined) {
			auto finalNode = stack.back();
			stack.pop_back();
			stack.back()->doRemove(finalNode, stack);
		}
	}

	template <bool IS_LEAF, typename Key, typename Value, uint16_t BtreeOrder>
	void
	getPrevious(BASE*, const vector<BASE*>&, BASE*&);
	
	template <bool IS_LEAF, typename Key, typename Value, uint16_t BtreeOrder>
	void
	getNext(BASE*, const vector<BASE*>&, BASE*&);
	/**
	 * @return combined or not
	 */
	NODE_TEMPLATE
	template <bool IS_LEAF>
	bool
	BASE::reBalance(const vector<NodeBase*>& passedNodeTrackStack)
	{
		auto& stack = passedNodeTrackStack;

		constexpr auto ceil = 1 + ((BtreeOrder - 1) / 2);
		if (childCount() < ceil) {
			NodeBase* previous = nullptr;
			// TODO should save search track？
			getPrevious<IS_LEAF>(this, stack, previous);
			NodeBase* next = nullptr;
			getNext<IS_LEAF>(this, stack, next);

			auto preHandler = [&] (NodeBase* pre) {
				return reBalanceWithPre<IS_LEAF>(pre, stack);
			};
			auto currentHandler = [] () { return false; };
			auto nxtHandler = [&] (NodeBase* nxt) {
				return reBalanceWithNxt<IS_LEAF>(nxt, stack);
			};

			return chooseBalanceNodeDo(previous, std::move(preHandler), this, std::move(currentHandler), next, std::move(nxtHandler));
		}

		return false;
	}

	NODE_TEMPLATE
	bool
	BASE::chooseBalanceNodeDo(NodeBase *pre,     function<bool(NodeBase*)> preHandler,
							  NodeBase *current, function<bool()> currentHandler,
							  NodeBase *nxt,     function<bool(NodeBase*)> nxtHandler) const
	{
		auto nullPre = (pre == nullptr);
		auto nullNxt = (nxt == nullptr);
		if (nullPre || nullNxt) {
			if (nullPre && nullNxt) {
				return currentHandler();
			} else if (nullPre) {
				return nxtHandler(nxt);
			} else {
				return preHandler(pre);
			}
		}

		auto preChilds = pre->childCount();
		auto curChilds = current->childCount();
		auto nxtChilds = nxt->childCount();
		auto average = (preChilds + curChilds + nxtChilds) / 3;

		return abs(preChilds - average) > abs(nxtChilds - average) ? preHandler(pre) : nxtHandler(nxt);
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
		// attention func change the stack or not

		auto oldMaxKey = previousNode->maxKey();
		auto previousTrackStack = getSiblingSearchTrackIn(stack, previousNode); // previous is leaf
		previousNode->elements_.append(std::move(appendPair));
		auto& newMaxKey = previousNode->maxKey(); // will change previous max
		changeMaxKeyUpper(previousTrackStack, newMaxKey);

		return true;
	}

	/**
	 * @return the complete search stack
	 */
	NODE_TEMPLATE
	vector<BASE*>
	BASE::getSiblingSearchTrackIn(const vector<NodeBase *>& currentNodePassedTrackStack, const NodeBase * sibling)
	{
		// TODO when call on Middle, there are duplicates compute
		// Because the last Middle has already search this(wait to verify)
		auto& stack = currentNodePassedTrackStack;
		auto& maxKey = sibling->maxKey();
		auto rCurrentNodeIter = ++stack.rbegin(); // from not leaf
		auto rEnd = stack.rend();

		vector<NodeBase*> trackStack;
		trackStack.reserve(stack.size());

		while (rCurrentNodeIter != rEnd) {
			// same ancestor of this and previous node
			if (ptrOff(rCurrentNodeIter)->have(maxKey, trackStack)) {
				break;
			}
			trackStack.clear();

			++rCurrentNodeIter;
		}
		trackStack.insert(trackStack.begin(), stack.begin(), rCurrentNodeIter.base()); // end is excluded

		trackStack.shrink_to_fit();
		return std::move(trackStack);
	}

	/**
	 * Key type should be copyable. Will not change stack. Last one in stack should be leaf actually
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
			auto matchIndex = ptrOff(upperNodeIter)->elements_.changeKeyOf(*rCurrentIter, newMaxKey);

			auto maxIndex = ptrOff(upperNodeIter)->childCount() - 1;
			if (matchIndex != maxIndex) {
				break;
			}

			++rCurrentIter;
		}
	}

	template <bool IS_LEAF, typename Key, typename Value, uint16_t BtreeOrder>
	void
	setNewPreRelation(BASE*, BASE*);

	// first in: split leaf
	NODE_TEMPLATE
	template <typename T>
	void
	BASE::splitNode(pair<Key, T> p, vector<NodeBase*>& passedNodeTrackStack)
	{
		auto& key = p.first;
		auto& lessThan = *(elements_.LessThanPtr);
		auto& stack = passedNodeTrackStack;

		auto newPre = this->clone();
		auto newPrePtr = newPre.get();
		// left is newPre, right is this
		setNewPreRelation<std::is_same<typename std::decay<T>::type, Value>::value>(newPrePtr, this);

		auto removeItems = [&] (uint16_t preNodeRemoveCount) {
			newPrePtr->elements_.template removeItems<false>(preNodeRemoveCount);
			this->elements_.template removeItems<true>(BtreeOrder - preNodeRemoveCount);
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
		auto i = elements_.suitPosition(key);
		if (i <= middle) {
			constexpr auto removeCount = middle;
			removeItems(removeCount);

			HANDLE_ADD(newPrePtr, middle);
		} else {
			constexpr auto removeCount = BtreeOrder - middle;
			removeItems(removeCount);

			HANDLE_ADD(this, BtreeOrder);
			if (shouldAppend) {
				changeMaxKeyUpper(stack, key);
			}
		}
		
#undef HANDLE_ADD

		insertNewPreToUpper(std::move(newPre), stack);
	}

	NODE_TEMPLATE
	void
	BASE::insertNewPreToUpper(unique_ptr<NodeBase> preNode, vector<NodeBase*>& passedNodeTrackStack)
	{

		auto& stack = passedNodeTrackStack;
		stack.pop_back(); // reduce lower node, prepare to new upper level add

		if (stack.empty()) { // means arrive root node
			auto& newLeftSonOfRoot = preNode;
			auto newRightSonOfRoot = this->move();
			this->elements_.append(make_pair<Key, unique_ptr<NodeBase>>(copy(newLeftSonOfRoot->maxKey()),
				                                                        std::move(newLeftSonOfRoot)));
			this->elements_.append(make_pair<Key, unique_ptr<NodeBase>>(copy(newRightSonOfRoot->maxKey()),
				                                                        std::move(newRightSonOfRoot)));
		} else {
			auto pair = make_pair<Key, unique_ptr<NodeBase>>(copy(preNode->maxKey()), std::move(preNode));
			stack.back()->doAdd(std::move(pair), stack);
		}
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

		getPrevious<std::is_same<T, Value>::value>(this, stack, previous); // some don't have one of siblings

		if (spaceFreeIn(previous)) {
			// if not free, will not trigger move, so the type is ref
			auto maxChanged = false;
			auto min = elements_.exchangeMin(std::move(p), maxChanged);
			if (maxChanged) {
				changeMaxKeyUpper(stack, maxKey());
			}

			return reallocatePre(previous, stack, std::move(min)); // trigger optimize
		}

		return false;
	}

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
		getNext<std::is_same<Value, T>::value>(this, stack, next); // some don't have one of siblings

		if (spaceFreeIn(next)) {
			// if not free, will not trigger move, so the ref type is fine
			auto&& oldMax = elements_.exchangeMax(std::move(p));
			changeMaxKeyUpper(stack, maxKey());

			return reallocateNxt(next, std::move(oldMax));
		}

		return false;
	}

	template <bool IS_LEAF, typename Key, typename Value, uint16_t BtreeOrder>
	void
	setRemoveCurrentRelation(BASE*);

	NODE_TEMPLATE
	template <bool IS_LEAF>
	bool
	BASE::reBalanceWithPre(NodeBase* previous, const vector<NodeBase*>& passedNodeTrackStack)
	{
		auto& stack = passedNodeTrackStack;

		// 这些使用 NodeBase 的操作会不会没有顾及到实际的类型，符合想要的语义吗？应该是符合的。Btree本来就是交给NodeBase来操作
		// 那 MiddleNode 和 LeafNode 的意义是什么呢？
		if (previous->childCount() + childCount() <= BtreeOrder) {
			// combine
			previous->receive(TailAppendWay(), std::move(*this));
			auto preStack = getSiblingSearchTrackIn(stack, previous);
			previous->changeMaxKeyUpper(preStack, previous->maxKey());
			setRemoveCurrentRelation<IS_LEAF>(this);

			return true;
		} else {
			// move some from pre to this
			auto moveCount = (previous->childCount() - childCount()) / 2;
			receive(HeadInsertWay(), moveCount, *previous);
			auto preStack = getSiblingSearchTrackIn(stack, previous);
			previous->changeMaxKeyUpper(preStack, previous->maxKey());

			return false;
		}
	}

	NODE_TEMPLATE
	template <bool IS_LEAF>
	bool
	BASE::reBalanceWithNxt(NodeBase* next, const vector<NodeBase*>& passedNodeTrackStack)
	{
		auto& stack = passedNodeTrackStack;

		if (next->childCount() + childCount() <= BtreeOrder) {
			// combine
			next->receive(HeadInsertWay(), std::move(*this));
			setRemoveCurrentRelation<IS_LEAF>(this);

			return true;
		} else {
			// move some from nxt to this
			auto moveCount = (next->childCount() - childCount()) / 2;
			receive(TailAppendWay(), moveCount, *next);
			changeMaxKeyUpper(stack, maxKey());

			return false;
		}
	}

	NODE_TEMPLATE
	void
	BASE::receive(TailAppendWay, NodeBase&& that)
	{
		elements_.receive(TailAppendWay(), std::move(that.elements_));
	}

	NODE_TEMPLATE
	void
	BASE::receive(HeadInsertWay, NodeBase&& that)
	{
		elements_.receive(HeadInsertWay(), std::move(that.elements_));
	}

	NODE_TEMPLATE
	void
	BASE::receive(HeadInsertWay, uint16_t count, NodeBase& node)
	{
		elements_.receive(HeadInsertWay(), count, node.elements_);
	}

	NODE_TEMPLATE
	void
	BASE::receive(TailAppendWay, uint16_t count, NodeBase& node)
	{
		elements_.receive(TailAppendWay(), count, node.elements_);
	}
#undef BASE
#undef NODE_TEMPLATE
}
