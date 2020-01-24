#pragma once
#include <vector>
#include "Basic.hpp"
#include "Util.hpp"
#include "Enumerator.hpp"
#include "Exception.hpp"
#include "Elements.hpp"

namespace Collections
{
	using ::std::move;

	namespace 
	{
		enum RetValue
		{
			Bool,
			SearchValue,
			Void,
		};
	}

	template <typename Key, typename Value, order_int BtreeOrder>
	class NodeBase
	{
	private:
		// TODO function<void(LeafNode*, LeafNode*)> _upNodeCallback;
	public:
		// TODO below two lines code wait to delete
		using Ele = Elements<Key, Value, BtreeOrder>;
		using LessThan = typename Ele::LessThan;

		virtual unique_ptr<NodeBase> Clone() const = 0;
		virtual unique_ptr<NodeBase> Move() const = 0;
		virtual ~NodeBase() = default;
		virtual bool Middle() const = 0;
		virtual vector<Key> Keys() const = 0;
		virtual Key MinKey() const = 0;
		virtual void Add(pair<Key, Value>) = 0;
		virtual void Remove(Key const&) = 0;

		bool ContainsKey(Key const& key, vector<NodeBase*>& passedNodeTrackStack)
		{
			auto& stack = passedNodeTrackStack;
			// only need to collect "don't have" situation
			auto collect = [&stack](NodeBase* node)
			{
				stack.push_back(node);
			};
			function<bool(NodeBase*)> collectDeepMaxOnBeyond = [&](NodeBase* node)
			{
				if (node->Middle())
				{
					auto maxIndex = node->ChildCount() - 1;
					auto maxChildPtr = Ele::ptr(node->elements_[maxIndex].second);
					collect(maxChildPtr);
					return collectDeepMaxOnBeyond(maxChildPtr);
				}

				return false;
			};
			auto trueOnEqual = [](auto) { return true; };

			return FindHelper<RetValue::Bool>(key, collect, trueOnEqual, collectDeepMaxOnBeyond);
		}

		bool ContainsKey(Key const& key) const
		{
			if (ChildCount() == 0) { return false; }
			return const_cast<NodeBase*>(this)->FindHelper<RetValue::Bool>(key, [](auto) { return true; });
		}

		Value GetValue(Key const& key) const
		{
			function<Value(NodeBase*)> moveDeepOnEqual = [&](NodeBase* node)
			{
				if (node->Middle())
				{
					auto maxIndex = node->ChildCount() - 1;
					return moveDeepOnEqual(Ele::ptr(node->elements_[maxIndex].second));
				}

				return node->elements_[key];
			};

			return const_cast<NodeBase*>(this)->FindHelper<RetValue::SearchValue>(key, moveDeepOnEqual);
		}

		void ModifyValue(Key const& key, Value value)
		{
			function<void(NodeBase*)> moveDeepOnEqual = [value = move(value), &moveDeepOnEqual, &key](NodeBase* node)
			{
				if (node->Middle())
				{
					auto maxIndex = node->ChildCount() - 1;
					auto maxChildPtr = Ele::ptr(node->elements_[maxIndex].second);
					moveDeepOnEqual(maxChildPtr);
					return;
				}

				node->elements_[key] = move(value);
			};

			FindHelper<RetValue::Void>(key, moveDeepOnEqual);
		}

		//void Add(pair<Key, Value> p, vector<NodeBase*>& passedNodeTrackStack)
		//{
		//	auto& stack = passedNodeTrackStack;
		//	auto lastNode = stack.back();
		//	lastNode->DoAdd(move(p), stack);
		//}

		//void Remove(Key const& key, vector<NodeBase*>& passedNodeTrackStack)
		//{
		//	auto& stack = passedNodeTrackStack;
		//	NodeBase* lastNode = stack.back();
		//	lastNode->doRemove(key, stack);
		//}

	protected:

		template <RetValue ReturnValue, typename T>
		virtual auto FindHelper(Key const& key, function<T(NodeBase*)> onEqualDo)
		{
			auto& lessThan = *(elements_.LessThanPtr);
#define ON_NOT_FOUND if constexpr (ReturnValue == RetValue::Bool) { return false; } else { throw KeyNotFoundException(); }

			function<T(NodeBase*)> imp = [onEqualDo = move(onEqualDo), &key, &imp, &lessThan](NodeBase* node)
			{
				for (auto& e : node->elements_)
				{
					if (lessThan(key, e.first))
					{
						if (node->Middle())
						{
							return imp(Ele::ptr(e.second));
						}
						else
						{
							ON_NOT_FOUND;
						}
					}
					else if (!lessThan(e.first, key))
					{
						return onEqualDo(Ele::ptr(e.second));
					}
				}

				ON_NOT_FOUND;
			};

			return imp(this);
#undef ON_NOT_FOUND 
		}

		// TODO add not only key-value add, but also key-unique_ptr add
		//template <typename T>
		//void DoAdd(pair<Key, T> p, vector<NodeBase*>& passedNodeTrackStack)
		//{
		//	auto& stack = passedNodeTrackStack;
		//	if (!Full())
		//	{
		//		if (auto maxChange = elements_.Add(move(p)))
		//		{
		//			ChangeMaxKeyFromBottomToRoot(stack, MinKey());
		//		}
		//	}

		//	tryPreviousAdd<T>(move(p), stack, tryNextAdd<T>, splitNode<T>);
		//}

		template <bool IsLeaf, typename Key, typename Value, order_int BtreeOrder>
		void getPrevious(NodeBase*, const vector<NodeBase*>&, NodeBase*&);

		// return means succeed or not
		//template <typename T>
		//void tryPreviousAdd(pair<Key, T> p, vector<NodeBase*>& passedNodeTrackStack, 
		//					function<void(pair<Key, T>, vector<NodeBase*>&, function<void(pair<Key, T>, vector<NodeBase*>&)>)> continuation1,
		//					function<void(pair<Key, T>, vector<NodeBase*>&)> continuation2)
		//{
		//	auto& stack = passedNodeTrackStack;
		//	NodeBase* previous = nullptr;
		//	getPrevious<std::is_same<T, Value>::value>(this, stack, previous); // some don't have one of siblings

		//	if (spaceFreeIn(previous))
		//	{
		//		// if not free, will not trigger move, so the type is ref
		//		auto maxChanged = false;
		//		auto min = elements_.ExchangeMin(move(p), maxChanged);
		//		if (maxChanged)
		//		{
		//			ChangeMaxKeyFromBottomToRoot(stack, MinKey());
		//		}

		//		return reallocatePre(previous, stack, move(min)); // trigger optimize
		//		// TODO up code meanings?
		//	}

		//	continuation1(move(p), stack, continuation2);
		//}

		template <bool IsLeaf, typename Key, typename Value, order_int BtreeOrder>
		void getNext(NodeBase*, const vector<NodeBase*>&, NodeBase*&);

		// return means succeed or not
		//template <typename T>
		//void tryNextAdd(pair<Key, T> p, vector<NodeBase*>& passedNodeTrackStack, function<void(pair<Key, T>, vector<NodeBase*>&)> continuation)
		//{
		//	auto& stack = passedNodeTrackStack;
		//	NodeBase* next = nullptr;
		//	getNext<std::is_same<Value, T>::value>(this, stack, next); // some don't have one of siblings

		//	if (spaceFreeIn(next))
		//	{
		//		// if not free, will not trigger move, so the ref type is fine
		//		auto&& oldMax = elements_.ExchangeMax(move(p));
		//		ChangeMaxKeyFromBottomToRoot(stack, MinKey());
		//		return reallocateNxt(next, move(oldMax));
		//	}

		//	// TODO also should think of up return value
		//	continuation(move(p), passedNodeTrackStack);
		//}

		template <bool IsLeaf, typename Key, typename Value, order_int BtreeOrder>
		void setNewPreRelation(NodeBase*, NodeBase*);

		// first in: split leaf
		template <typename T>
		void splitNode(pair<Key, T> p, vector<NodeBase*>& passedNodeTrackStack)
		{
			auto& key = p.first;
			auto& lessThan = *(elements_.LessThanPtr);
			auto& stack = passedNodeTrackStack;

			auto newPre = this->Clone();
			auto newPrePtr = newPre.get();
			// left is newPre, right is this
			setNewPreRelation<std::is_same<typename std::decay<T>::type, Value>::value>(newPrePtr, this);

			auto removeItems = [&](order_int preNodeRemoveCount)
			{
				newPrePtr->elements_.template RemoveItems<false>(preNodeRemoveCount);
				this->elements_.template RemoveItems<true>(BtreeOrder - preNodeRemoveCount);
			};

			auto addIn = [&](NodeBase* node, bool shouldAppend)
			{
				if (shouldAppend)
				{
					node->elements_.Append(move(p));
				}
				else
				{
					node->elements_.Insert(move(p));
				}
			};

#define HANDLE_ADD(leaf, maxBound) auto shouldAppend = (i == maxBound); do { addIn(leaf, shouldAppend); } while(0)
			constexpr bool odd = BtreeOrder % 2;
			constexpr auto middle = odd ? (BtreeOrder / 2 + 1) : (BtreeOrder / 2);
			auto i = elements_.SuitPosition(key);
			if (i <= middle)
			{
				constexpr auto removeCount = middle;
				removeItems(removeCount);

				HANDLE_ADD(newPrePtr, middle);
			}
			else
			{
				constexpr auto removeCount = BtreeOrder - middle;
				removeItems(removeCount);

				HANDLE_ADD(this, BtreeOrder);
				if (shouldAppend)
				{
					ChangeMaxKeyFromBottomToRoot(stack, key);
				}
			}
#undef HANDLE_ADD

			insertNewPreToUpper(move(newPre), stack);
		}

		//template <typename T>
		//bool reallocatePre(NodeBase* previousNode, vector<NodeBase*>& passedNodeTrackStack, pair<Key, T> appendPair)
		//{
		//	auto& stack = passedNodeTrackStack;
		//	// attention func change the stack or not

		//	auto oldMaxKey = previousNode->MinKey();
		//	auto previousTrackStack = getSiblingSearchTrackIn(stack, previousNode); // previous is leaf
		//	previousNode->elements_.Append(move(appendPair));
		//	auto newMaxKey = previousNode->MinKey(); // will change previous max
		//	ChangeMaxKeyFromBottomToRoot(previousTrackStack, newMaxKey);

		//	return true;
		//}

		//template <typename T>
		//bool reallocateNxt(NodeBase* nextNode, pair<Key, T> insertPair)
		//{
		//	nextNode->elements_.Insert(move(insertPair));
		//	return true;
		//}

		// void ChangeMaxKeyFromBottomToRoot(vector<NodeBase*> const& passedNodeTrackStack, Key const& newMaxKey) const
		// {
		// 	if (passedNodeTrackStack.size() == 1) { return; }

		// 	auto& stack = passedNodeTrackStack;
		// 	auto rCurrentIter = stack.rbegin();
		// 	auto rEnd = stack.rend();
		// 	for (auto upperNodeIter = rCurrentIter + 1; rCurrentIter != rEnd; ++rCurrentIter, upperNodeIter = rCurrentIter + 1)
		// 	{
		// 		// Up node here is MiddleNode already
		// 		auto matchIndex = ptrOff(upperNodeIter)->elements_.ChangeKeyOf(*rCurrentIter, newMaxKey);
		// 		auto maxIndex = ptrOff(upperNodeIter)->ChildCount() - 1;
		// 		if (matchIndex != maxIndex) { break; }
		// 	}
		// }

		//void insertNewPreToUpper(unique_ptr<NodeBase> preNode, vector<NodeBase*>& passedNodeTrackStack)
		//{

		//	auto& stack = passedNodeTrackStack;
		//	stack.pop_back(); // reduce lower node, prepare to new upper level add

		//	if (stack.empty())
		//	{ // means arrive root node
		//		auto& newLeftSonOfRoot = preNode;
		//		auto newRightSonOfRoot = this->Move();
		//		this->elements_.Append(make_pair<Key, unique_ptr<NodeBase>>(copy(newLeftSonOfRoot->MinKey()),
		//																	move(newLeftSonOfRoot)));
		//		this->elements_.Append(make_pair<Key, unique_ptr<NodeBase>>(copy(newRightSonOfRoot->MinKey()),
		//																	move(newRightSonOfRoot)));
		//	}
		//	else
		//	{
		//		auto pair = make_pair<Key, unique_ptr<NodeBase>>(copy(preNode->MinKey()), move(preNode));
		//		stack.back()->DoAdd(move(pair), stack);
		//	}
		//}
		

		template <typename T>
		inline void doRemove(const T& t, vector<NodeBase*>& passedNodeTrackStack)
		{
			auto& stack = passedNodeTrackStack;

			auto i = elements_.IndexOf(t);
			if (elements_.RemoveAt(i))
			{
				ChangeMaxKeyFromBottomToRoot(stack, MinKey());
			}

			// TODO is_same need to verify
			auto combined = reBalance<std::is_same<T, Key>::value>(stack);
			if (combined)
			{
				auto finalNode = stack.back();
				// TODO judge if arrive root?
				stack.pop_back();
				stack.back()->doRemove(finalNode, stack);
			}
		}

		// return combined or not
		template <bool IsLeaf>
		bool reBalance(const vector<NodeBase*>& passedNodeTrackStack)
		{
			auto& stack = passedNodeTrackStack;

			constexpr auto ceil = 1 + ((BtreeOrder - 1) / 2);
			if (ChildCount() < ceil)
			{
				NodeBase* previous = nullptr;
				// TODO should save search track？
				getPrevious<IsLeaf>(this, stack, previous);
				NodeBase* next = nullptr;
				getNext<IsLeaf>(this, stack, next);

				auto preHandler = [&](NodeBase* pre)
				{
					return reBalanceWithPre<IsLeaf>(pre, stack);
				};
				auto currentHandler = []() { return false; };
				auto nxtHandler = [&](NodeBase* nxt)
				{
					return reBalanceWithNxt<IsLeaf>(nxt, stack);
				};

				return chooseBalanceNodeDo(previous, move(preHandler), this, move(currentHandler), next, move(nxtHandler));
			}

			return false;
		}

		bool chooseBalanceNodeDo(NodeBase* pre, function<bool(NodeBase*)> preHandler,
								 NodeBase* current, function<bool()> currentHandler,
								 NodeBase* nxt, function<bool(NodeBase*)> nxtHandler) const
		{
			auto nullPre = (pre == nullptr);
			auto nullNxt = (nxt == nullptr);
			if (nullPre || nullNxt)
			{
				if (nullPre && nullNxt)
				{
					return currentHandler();
				}
				else if (nullPre)
				{
					return nxtHandler(nxt);
				}
				else
				{
					return preHandler(pre);
				}
			}

			auto preChilds = (long)pre->ChildCount();
			auto curChilds = (long)current->ChildCount();
			auto nxtChilds = (long)nxt->ChildCount();
			auto average = (preChilds + curChilds + nxtChilds) / 3;

			return abs(preChilds - average) > abs(nxtChilds - average) ? preHandler(pre) : nxtHandler(nxt);
		}

		template <bool IsLeaf>
		bool reBalanceWithPre(NodeBase* previous, const vector<NodeBase*>& passedNodeTrackStack)
		{
			auto& stack = passedNodeTrackStack;

			// 这些使用 NodeBase 的操作会不会没有顾及到实际的类型，符合想要的语义吗？应该是符合的。Btree本来就是交给NodeBase来操作
			// 那 MiddleNode 和 LeafNode 的意义是什么呢？
			if (previous->ChildCount() + ChildCount() <= BtreeOrder)
			{
				// combine
				previous->Receive(TailAppendWay(), move(*this));
				auto preStack = getSiblingSearchTrackIn(stack, previous);
				previous->ChangeMaxKeyFromBottomToRoot(preStack, previous->MinKey());
				setRemoveCurrentRelation<IsLeaf>(this);

				return true;
			}
			else
			{
				// move some from pre to this
				auto moveCount = (previous->ChildCount() - ChildCount()) / 2;
				Receive(HeadInsertWay(), moveCount, *previous);
				auto preStack = getSiblingSearchTrackIn(stack, previous);
				previous->ChangeMaxKeyFromBottomToRoot(preStack, previous->MinKey());

				return false;
			}
		}

		template <bool IsLeaf, typename Key, typename Value, order_int BtreeOrder>
		void setRemoveCurrentRelation(NodeBase*);

		template <bool IsLeaf>
		bool reBalanceWithNxt(NodeBase* next, const vector<NodeBase*>& passedNodeTrackStack)
		{
			auto& stack = passedNodeTrackStack;
			if (next->ChildCount() + ChildCount() <= BtreeOrder)
			{
				// combine
				next->Receive(HeadInsertWay(), move(*this));
				setRemoveCurrentRelation<IsLeaf>(this);
				return true;
			}
			else
			{
				// move some from nxt to this
				auto moveCount = (next->ChildCount() - ChildCount()) / 2;
				Receive(TailAppendWay(), moveCount, *next);
				ChangeMaxKeyFromBottomToRoot(stack, MinKey());
				return false;
			}
		}

		void Receive(TailAppendWay, NodeBase&& that)
		{
			elements_.Receive<false>(move(that.elements_));
		}

		void Receive(HeadInsertWay, NodeBase&& that)
		{
			elements_.Receive<true>(move(that.elements_));
		}

		void Receive(HeadInsertWay, uint16_t count, NodeBase& node)
		{
			elements_.Receive(HeadInsertWay(), count, node.elements_);
		}

		void Receive(TailAppendWay, uint16_t count, NodeBase& node)
		{
			elements_.Receive(TailAppendWay(), count, node.elements_);
		}

		static bool spaceFreeIn(NodeBase const* node)
		{
			if (node == nullptr) { return false; }
			return !node->Full();
		}

		// return the complete search 
		static vector<NodeBase*> getSiblingSearchTrackIn(vector<NodeBase*> const& currentNodePassedTrackStack,
														 NodeBase const* sibling)
		{
			// TODO when call on Middle, there are duplicates compute
			// Because the last Middle has already search this(wait to verify)
			auto& stack = currentNodePassedTrackStack;
			auto maxKey = sibling->MinKey();
			auto rCurrentNodeIter = ++stack.rbegin(); // from not leaf
			auto rEnd = stack.rend();

			vector<NodeBase*> trackStack;
			trackStack.reserve(stack.size());

			while (rCurrentNodeIter != rEnd)
			{
				// same ancestor of this and previous node
				if (ptrOff(rCurrentNodeIter)->ContainsKey(maxKey, trackStack))
				{
					break;
				}
				trackStack.clear();

				++rCurrentNodeIter;
			}
			trackStack.insert(trackStack.begin(), stack.begin(), rCurrentNodeIter.base()); // end is excluded

			trackStack.shrink_to_fit();
			return move(trackStack);
		}
	};
}
