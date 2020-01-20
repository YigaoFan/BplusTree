#pragma once
#include <vector>
#include "Basic.hpp"
#include "Util.hpp"
#include "Exception.hpp"
#include "Elements.hpp"

namespace Collections
{
	using ::std::move;

	enum RetValue
	{
		Bool,
		SearchValue,
		Void,
	};

	template <typename Key, typename Value, order_int BtreeOrder>
	class NodeBase
	{
	protected:
		Elements<Key, Value, BtreeOrder, NodeBase> elements_;
	public:
		using Ele = Elements<Key, Value, BtreeOrder, NodeBase>;
		using LessThan = typename Ele::LessThan;

		template <typename Iter>
		NodeBase(Iter begin, Iter end, shared_ptr<LessThan> lessThanPtr)
			: elements_(begin, end, lessThanPtr)
		{}

		template <typename T, typename Iterator>
		NodeBase(Enumerator<pair<Key, T>, Iterator> enumerator, shared_ptr<LessThan> lessThan)
			: elements_(enumerator, lessThan)
		{}

		NodeBase(NodeBase const& that)
			: elements_(that.elements_)
		{}

		NodeBase(NodeBase&& that) noexcept
			: elements_(move(that.elements_))
		{}

		virtual unique_ptr<NodeBase> Clone() const = 0;
		virtual unique_ptr<NodeBase> Move() const = 0;
		virtual ~NodeBase() = default;
		virtual vector<Key> Keys() const = 0;

		bool Middle() const
		{
			return elements_.MiddleFlag;
		}

		Key MaxKey() const
		{
			return elements_[elements_.Count() - 1].first;
		}

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

				node->elements_[key] = value;
			};

			FindHelper<RetValue::Void>(key, moveDeepOnEqual);
		}

		void Add(pair<Key, Value> p, vector<NodeBase*>& passedNodeTrackStack)
		{
			auto& stack = passedNodeTrackStack;
			auto lastNode = stack.back();
			lastNode->DoAdd(move(p), stack);
		}

		void Remove(Key const& key, vector<NodeBase*>& passedNodeTrackStack)
		{
			auto& stack = passedNodeTrackStack;
			NodeBase* lastNode = stack.back();
			lastNode->doRemove(key, stack);
		}

	protected:
		order_int ChildCount() const
		{
			return elements_.Count();
		}

		bool Full() const
		{
			return elements_.Full();
		}

		template <RetValue ReturnValue, typename T>
		auto FindHelper(Key const& key, function<T(NodeBase*)> onEqualDo)
		{
			auto& lessThan = *(elements_.LessThanPtr);
#define ON_NOT_FOUND if constexpr (ReturnValue == RetValue::Bool) { return false; } else { throw KeyNotFoundException(); }

			function<T(NodeBase*)> imp = [equalHandler = move(onEqualDo), &key, &imp, &lessThan](NodeBase* node)
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
		template <typename T>
		void DoAdd(pair<Key, T> p, vector<NodeBase*>& passedNodeTrackStack)
		{
			auto& k = p.first;
			auto& stack = passedNodeTrackStack;
			auto& lessThan = *(elements_.LessThanPtr);

			if (!Full())
			{
				if (elements_.Add(move(p))) // true means max key changed
				{
					changeMaxKeyUpper(stack, k);
				}
			}
			else
			{
				// maybe fine the choose of pre and next
				if (tryPreviousAdd<T>(p, stack))
				{

				}
				else if (tryNextAdd<T>(p, stack))
				{

				}
				else
				{
					splitNode(move(p), stack);
				}
			}
		}

		template <bool IS_LEAF, typename Key, typename Value, order_int BtreeOrder>
		void getPrevious(NodeBase*, const vector<NodeBase*>&, NodeBase*&);

		// return means succeed or not
		template <typename T>
		bool tryPreviousAdd(pair<Key, T>& p, vector<NodeBase*>& passedNodeTrackStack)
		{
			auto& stack = passedNodeTrackStack;
			NodeBase* previous = nullptr;

			getPrevious<std::is_same<T, Value>::value>(this, stack, previous); // some don't have one of siblings

			if (spaceFreeIn(previous))
			{
				// if not free, will not trigger move, so the type is ref
				auto maxChanged = false;
				auto min = elements_.ExchangeMin(move(p), maxChanged);
				if (maxChanged)
				{
					changeMaxKeyUpper(stack, MaxKey());
				}

				return reallocatePre(previous, stack, move(min)); // trigger optimize
			}

			return false;
		}

		template <bool IS_LEAF, typename Key, typename Value, order_int BtreeOrder>
		void getNext(NodeBase*, const vector<NodeBase*>&, NodeBase*&);

		// return means succeed or not
		template <typename T>
		bool tryNextAdd(pair<Key, T>& p, vector<NodeBase*>& passedNodeTrackStack)
		{
			auto& stack = passedNodeTrackStack;

			NodeBase* next = nullptr;
			getNext<std::is_same<Value, T>::value>(this, stack, next); // some don't have one of siblings

			if (spaceFreeIn(next))
			{
				// if not free, will not trigger move, so the ref type is fine
				auto&& oldMax = elements_.ExchangeMax(move(p));
				changeMaxKeyUpper(stack, MaxKey());

				return reallocateNxt(next, move(oldMax));
			}

			return false;
		}

		template <bool IS_LEAF, typename Key, typename Value, order_int BtreeOrder>
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

			auto removeItems = [&](uint16_t preNodeRemoveCount)
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
					changeMaxKeyUpper(stack, key);
				}
			}

#undef HANDLE_ADD

			insertNewPreToUpper(move(newPre), stack);
		}

		template <typename T>
		bool reallocatePre(NodeBase* previousNode, vector<NodeBase*>& passedNodeTrackStack, pair<Key, T> appendPair)
		{
			auto& stack = passedNodeTrackStack;
			// attention func change the stack or not

			auto oldMaxKey = previousNode->MaxKey();
			auto previousTrackStack = getSiblingSearchTrackIn(stack, previousNode); // previous is leaf
			previousNode->elements_.Append(move(appendPair));
			auto newMaxKey = previousNode->MaxKey(); // will change previous max
			changeMaxKeyUpper(previousTrackStack, newMaxKey);

			return true;
		}

		template <typename T>
		bool reallocateNxt(NodeBase* nextNode, pair<Key, T> insertPair)
		{
			nextNode->elements_.Insert(move(insertPair));
			return true;
		}

		// Key type should be copyable.Will not change stack.Last one in stack should be leaf actually
		void changeMaxKeyUpper(const vector<NodeBase*>& passedNodeTrackStack, const Key& newMaxKey) const
		{
			auto& stack = passedNodeTrackStack;
			auto rCurrentIter = stack.rbegin();
			auto rEnd = stack.rend();

			if ((rEnd - rCurrentIter) == 1)
			{
				return;
			}

			while (rCurrentIter != rEnd)
			{
				auto upperNodeIter = rCurrentIter + 1;
				auto matchIndex = ptrOff(upperNodeIter)->elements_.ChangeKeyOf(*rCurrentIter, newMaxKey);

				auto maxIndex = ptrOff(upperNodeIter)->ChildCount() - 1;
				if (matchIndex != maxIndex)
				{
					break;
				}

				++rCurrentIter;
			}
		}

		void insertNewPreToUpper(unique_ptr<NodeBase> preNode, vector<NodeBase*>& passedNodeTrackStack)
		{

			auto& stack = passedNodeTrackStack;
			stack.pop_back(); // reduce lower node, prepare to new upper level add

			if (stack.empty())
			{ // means arrive root node
				auto& newLeftSonOfRoot = preNode;
				auto newRightSonOfRoot = this->Move();
				this->elements_.Append(make_pair<Key, unique_ptr<NodeBase>>(copy(newLeftSonOfRoot->MaxKey()),
																			move(newLeftSonOfRoot)));
				this->elements_.Append(make_pair<Key, unique_ptr<NodeBase>>(copy(newRightSonOfRoot->MaxKey()),
																			move(newRightSonOfRoot)));
			}
			else
			{
				auto pair = make_pair<Key, unique_ptr<NodeBase>>(copy(preNode->MaxKey()), move(preNode));
				stack.back()->DoAdd(move(pair), stack);
			}
		}
		

		template <typename T>
		inline void doRemove(const T& t, vector<NodeBase*>& passedNodeTrackStack)
		{
			auto& stack = passedNodeTrackStack;

			auto i = elements_.IndexOf(t);
			if (elements_.RemoveAt(i))
			{
				changeMaxKeyUpper(stack, MaxKey());
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
		template <bool IS_LEAF>
		bool reBalance(const vector<NodeBase*>& passedNodeTrackStack)
		{
			auto& stack = passedNodeTrackStack;

			constexpr auto ceil = 1 + ((BtreeOrder - 1) / 2);
			if (ChildCount() < ceil)
			{
				NodeBase* previous = nullptr;
				// TODO should save search track？
				getPrevious<IS_LEAF>(this, stack, previous);
				NodeBase* next = nullptr;
				getNext<IS_LEAF>(this, stack, next);

				auto preHandler = [&](NodeBase* pre)
				{
					return reBalanceWithPre<IS_LEAF>(pre, stack);
				};
				auto currentHandler = []() { return false; };
				auto nxtHandler = [&](NodeBase* nxt)
				{
					return reBalanceWithNxt<IS_LEAF>(nxt, stack);
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

		template <bool IS_LEAF>
		bool reBalanceWithPre(NodeBase* previous, const vector<NodeBase*>& passedNodeTrackStack)
		{
			auto& stack = passedNodeTrackStack;

			// 这些使用 NodeBase 的操作会不会没有顾及到实际的类型，符合想要的语义吗？应该是符合的。Btree本来就是交给NodeBase来操作
			// 那 MiddleNode 和 LeafNode 的意义是什么呢？
			if (previous->ChildCount() + ChildCount() <= BtreeOrder)
			{
				// combine
				previous->receive(TailAppendWay(), move(*this));
				auto preStack = getSiblingSearchTrackIn(stack, previous);
				previous->changeMaxKeyUpper(preStack, previous->MaxKey());
				setRemoveCurrentRelation<IS_LEAF>(this);

				return true;
			}
			else
			{
				// move some from pre to this
				auto moveCount = (previous->ChildCount() - ChildCount()) / 2;
				receive(HeadInsertWay(), moveCount, *previous);
				auto preStack = getSiblingSearchTrackIn(stack, previous);
				previous->changeMaxKeyUpper(preStack, previous->MaxKey());

				return false;
			}
		}

		template <bool IS_LEAF, typename Key, typename Value, order_int BtreeOrder>
		void setRemoveCurrentRelation(NodeBase*);

		template <bool IS_LEAF>
		bool reBalanceWithNxt(NodeBase* next, const vector<NodeBase*>& passedNodeTrackStack)
		{
			auto& stack = passedNodeTrackStack;

			if (next->ChildCount() + ChildCount() <= BtreeOrder)
			{
				// combine
				next->receive(HeadInsertWay(), move(*this));
				setRemoveCurrentRelation<IS_LEAF>(this);

				return true;
			}
			else
			{
				// move some from nxt to this
				auto moveCount = (next->ChildCount() - ChildCount()) / 2;
				receive(TailAppendWay(), moveCount, *next);
				changeMaxKeyUpper(stack, MaxKey());

				return false;
			}
		}

		void receive(TailAppendWay, NodeBase&& that)
		{
			elements_.Receive<false>(move(that.elements_));
		}

		void receive(HeadInsertWay, NodeBase&& that)
		{
			elements_.Receive<true>(move(that.elements_));
		}

		void receive(HeadInsertWay, uint16_t count, NodeBase& node)
		{
			elements_.Receive(HeadInsertWay(), count, node.elements_);
		}

		void receive(TailAppendWay, uint16_t count, NodeBase& node)
		{
			elements_.Receive(TailAppendWay(), count, node.elements_);
		}

		static bool spaceFreeIn(NodeBase const* node)
		{
			if (node != nullptr)
			{
				return !node->Full();
			}

			return false;
		}

		// return the complete search 
		static vector<NodeBase*> getSiblingSearchTrackIn(const vector<NodeBase*>& currentNodePassedTrackStack, const NodeBase* sibling)
		{
			// TODO when call on Middle, there are duplicates compute
			// Because the last Middle has already search this(wait to verify)
			auto& stack = currentNodePassedTrackStack;
			auto maxKey = sibling->MaxKey();
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
