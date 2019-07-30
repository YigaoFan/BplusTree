#pragma once
#include <vector> // for vector
#include "Elements.hpp"
//#include "DoAdd.hpp"

namespace btree {
	using std::make_pair;

	struct LeafFlag   {};
	struct MiddleFlag {};

	template<typename Key, typename Value, uint16_t BtreeOrder, typename T>
	void
	extern doAdd(NodeBase<Key, Value, BtreeOrder>*, pair<Key, T>, vector<NodeBase<Key, Value, BtreeOrder>*>&);

#define NODE_TEMPLATE template <typename Key, typename Value, uint16_t BtreeOrder>

	NODE_TEMPLATE
	class NodeBase {
		template <typename T>
		friend void doAdd<Key, Value, BtreeOrder, T>(NodeBase*, pair<Key, T>, vector<NodeBase*>&);
	public:
		using Ele      = Elements<Key, Value, BtreeOrder, NodeBase>;
		using LessThan = typename Ele::LessThan;
		const bool Middle;
		template <typename Iter>
		NodeBase(LeafFlag,   Iter, Iter, shared_ptr<LessThan>);
		template <typename Iter>
		NodeBase(MiddleFlag, Iter, Iter, shared_ptr<LessThan>);
		NodeBase(const NodeBase&);
		NodeBase(NodeBase&&) noexcept;
		virtual unique_ptr<NodeBase> clone() const = 0;
		virtual ~NodeBase() = default;

		inline Key         maxKey() const;
		uint16_t           childCount() const;
		inline vector<Key> allKey() const;
		inline bool        have  (const Key&);
		Value*             search(const Key&);
		bool               add(pair<Key, Value>);
		bool               remove(const Key&);
		void searchSiblingsIn(vector<NodeBase *> &, NodeBase*&, NodeBase*&) const;

	protected:
		Elements<Key, Value, BtreeOrder, NodeBase> elements_;
		inline bool empty() const;
		inline bool full()  const;
		inline void   changeInSearchDownPath(const Key&, const Key&);
		inline Value* searchWithSaveTrack(const Key&, vector<NodeBase*>&);

		template <typename T>
		inline void reallocateSiblingElement(bool, NodeBase*, vector<NodeBase*>&, pair<Key, T>);
		inline void changeMaxKeyIn(vector<NodeBase*>&, const Key&) const;
		inline void replacePreviousNodeMaxKeyInTreeBySearchUpIn(vector<NodeBase*>&, const Key&, const Key&);
		template <typename T>
		inline void splitNode(pair<Key, T>, vector<NodeBase*>&);
		inline void insertLeafToUpper(NodeBase*, vector<NodeBase*>&) const;
	};
}

namespace btree {
#define BASE NodeBase<Key, Value, BtreeOrder>

	NODE_TEMPLATE
	template <typename Iter>
	BASE::NodeBase(LeafFlag, Iter begin, Iter end, shared_ptr<LessThan> lessThanPtr)
		: Middle(false), elements_(begin, end, lessThanPtr)
	{ }	
	NODE_TEMPLATE
	template <typename Iter>
	BASE::NodeBase(MiddleFlag, Iter begin, Iter end, shared_ptr<LessThan> funcPtr)
		: Middle(true), elements_(begin, end, funcPtr)
	{ }

	NODE_TEMPLATE
	BASE::NodeBase(const NodeBase& that)
		: Middle(that.Middle), elements_(that.elements_)
	{ }

	NODE_TEMPLATE
	BASE::NodeBase(NodeBase&& that) noexcept
		: Middle(that.Middle), elements_(std::move(that.elements_))
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
	Key
	BASE::maxKey() const
	{
		return elements_[elements_.count() - 1].first;
	}

	NODE_TEMPLATE
	bool
	BASE::have(const Key& key)
	{
		if (!Middle) {
			return elements_.have(key);
		} else {
			for (auto& e : elements_) {
				if ((*elements_.LessThanPtr)(key, e.first)) {
					Ele::ptr(e.second)->have(key);
				} else if (!(*elements_.LessThanPtr)(e.first, key)) {
					return true;
				}
			}

			return false;
		}
	}

	NODE_TEMPLATE
	Value*
	BASE::search(const Key& key)
	{
		auto maxValueForContent = [] (Ele& e) -> typename Ele::ValueForContent& {
			return e[e.count() - 1].second;
		};

		if (!Middle) {
			return elements_.have(key) ? &Ele::value(elements_[key]) : nullptr;
		} else {
			for (auto& e : elements_) {
				if ((*elements_.LessThanPtr)(key, e.first)) {
					return Ele::ptr(e.second)->search(key);
				} else if (!(*elements_.LessThanPtr)(e.first, key)) {
					auto subNodePtr = Ele::ptr(e.second);

				SearchInThisNode:
					if (!subNodePtr->Middle) {
						return &Ele::value(maxValueForContent(subNodePtr->elements_));
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

	template <typename Key, typename Value, uint16_t BtreeOrder>
	void
	collectDeepInfo(BASE*, const Key&, vector<BASE*>&);

	NODE_TEMPLATE
	bool
	BASE::add(pair<Key, Value> p)
	{
		vector<decltype(this)> passedNodeTrackStack;

		auto& key = p.first;
		// here is wrong
		collectDeepInfo(this, key, passedNodeTrackStack);
        // 模板参数匹配顺序
		doAdd<Key, Value, BtreeOrder, Value>(this, p, passedNodeTrackStack);

		return false; // TODO wait to modify
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
		if (!Middle) {
			elements_.remove(key);
			// check count
		} else {
			for (auto& e : elements_) {
				if ((*elements_.LessThanPtr)(key, e.first)) {
					// Recursive locate
				} else if (!(*elements_.LessThanPtr)(e.first, key)) {
					auto subNodePtr = Ele::ptr(e.second);

				SearchInThisNode:
					if (!subNodePtr->Middle) {
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

			auto i = node.elements_.indexOf(oldKey);
			if (i != -1 && node->Middle) { // No need to change leaf
				node.elements_[i].first = newKey;
			}
		}

	}

	NODE_TEMPLATE
	Value*
	BASE::searchWithSaveTrack(const Key& key, vector<NodeBase*>& trackStack)
	{
		// TODO modify, or think of search
		// TODO maybe not need return value
		trackStack.push_back(this);

		auto maxValueForContent = [] (Ele& e) {
			return e[e.count() - 1].second;
		};

		if (!Middle) {
			return elements_.have(key) ? &Ele::value(elements_[key]) : nullptr;
		} else {
			for (auto& e : elements_) {
				if ((*elements_.LessThanPtr)(key, e.first)) {
					return Ele::ptr(e.second)->searchWithSaveTrack(key, trackStack);
				} else if (!(*elements_.LessThanPtr)(e.first, key)) {
					auto subNodePtr = Ele::ptr(e.second);

				SearchInThisNode:
					trackStack.push_back(subNodePtr);

					if (!subNodePtr->Middle) {
						return &Ele::value(maxValueForContent(subNodePtr->elements_));
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
	void
	BASE::searchSiblingsIn(vector<NodeBase*>& passedNodeTrackStack, NodeBase*& previous, NodeBase*& next) const
	{

	}

	NODE_TEMPLATE
	template <typename T>
	void
	BASE::reallocateSiblingElement(bool isPrevious, NodeBase* sibling, vector<NodeBase*>& passedNodeTrackStack, pair<Key, T> p)
	{
		auto& stack = passedNodeTrackStack;
		
		if (isPrevious) {
			auto&& min = elements_.exchangeMin(p);
			auto&& previousOldMax = sibling->maxKey();
			sibling->elements_.append(min);
			// previous max change
			// TODO could use this stack to get sibling stack, then change upper
			replacePreviousNodeMaxKeyInTreeBySearchUpIn(stack, sibling, min);
		} else {
			auto&& max = elements_.exchangeMax(p);
			// this max change
			changeMaxKeyIn(stack, maxKey());
			sibling->elements_.insert(p);
		}
	}

	NODE_TEMPLATE
	void
	BASE::changeMaxKeyIn(vector<NodeBase*>& passedNodeTrackStack, const Key& maxKey) const
	{
		auto& stack = passedNodeTrackStack;
		if (stack.size() < 2) {
			return;
		}

		auto  nodePtr = stack.pop_back();
		auto& upperNode = *(stack[stack.size() - 1]);
		// 以下这个修改 key 的部分可以复用
		auto  matchIndex = upperNode.elements_.indexOf(nodePtr);
		upperNode[matchIndex].first = maxKey;

		auto maxIndex = upperNode->childCount() - 1;
		if (matchIndex == maxIndex) {
			changeMaxKeyIn(stack, maxKey);
		}
	}

	// use stack to get root node(maybe not need root node), then use this early max key to search
	NODE_TEMPLATE
	void
	BASE::replacePreviousNodeMaxKeyInTreeBySearchUpIn(vector<NodeBase*>& passedNodeTrackStack, const Key& oldKey, const Key& newKey)
	{
		auto& stack = passedNodeTrackStack;
#define EMIT_UPPER_NODE() stack.pop_back()

		EMIT_UPPER_NODE(); // top pointer is leaf, it's useless

		for (NodeBase* node = EMIT_UPPER_NODE(); stack.size() != 0; node = EMIT_UPPER_NODE()) {
			// judge if the node is the same ancestor between this and previous node
			if (node->have(oldKey)) {
				node->changeInSearchDownPath(oldKey, newKey);

				// judge if need to change upper node
				auto i = node->elements_.indexOf(oldKey); // should create a method called this layer search
				auto maxIndex = childCount() - 1;
				if (i != -1 && i == maxIndex) {
					stack.push_back(node);
					// change upper node
					changeMaxKeyIn(stack, newKey);
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
		auto& lessThan = *(Ele::LessThanPtr);
		auto& stack = passedNodeTrackStack;

		auto newPre = this->clone(); // TODO have a problem
		auto newPrePtr = newPre.get();
		// left is newPre, right is this
		setSiblings<T>(newPrePtr, this);

		auto i = elements_.suitablePosition(key);

		// [] not need reference if don't have last sentence?
		auto moveItems = [&] (uint16_t preNodeRemoveCount) {
			newPrePtr->elements_.removeItemsFrom(false, preNodeRemoveCount);
			this->elements_.removeItemsFrom(true,  BtreeOrder - preNodeRemoveCount);
		};

		auto addIn = [&] (NodeBase* node, bool shouldAppend) {
			if (shouldAppend) {
				node->elements_.append(p);
			} else {
				node->elements_.insert(p);
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
				changeMaxKeyIn(stackCopy, key);
			}
		}
		
#undef HANDLE_ADD

		insertLeafToUpper(newPrePtr, stack);
	}

	NODE_TEMPLATE
	void
	BASE::insertLeafToUpper(NodeBase* node, vector<NodeBase*>& passedNodeTrackStack) const
	{
#define EMIT_UPPER_NODE() stack.pop_back()

		// reduce useless node, prepare to upper level add, like start new leaf add
		auto& stack = passedNodeTrackStack;
		EMIT_UPPER_NODE();

		doAdd(make_pair<Key, NodeBase*>(node->maxKey(), node), stack);
		// 这部分甚至可能动 root，做好心理准备

#undef EMIT_UPPER_NODE
	}
#undef BASE
#undef NODE_TEMPLATE
}
