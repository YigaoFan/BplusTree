#pragma once
#include <memory>
#include <functional>
#include <type_traits>
#include "../Basic/Exception.hpp"
#include "../Basic/TypeTrait.hpp"
#include "../Basic/Debug.hpp"
#include "Basic.hpp"
#include "EnumeratorPipeline.hpp"
#include "Elements.hpp"
#include "NodeBase.hpp"
#include "LeafNode.hpp"
#include "ClonerDeclare.hpp"
#include "../FuncLib/Persistence/FriendFuncLibDeclare.hpp"
#include "../FuncLib/Persistence/PtrSetter.hpp"

namespace Collections 
{
	using ::Basic::Assert;
	using ::Basic::IsSpecialization; // PtrSetter use
	using ::std::bind;
	using ::std::make_pair;
	using ::std::move;
	using ::std::result_of_t;
	using ::std::placeholders::_1;
	using ::std::placeholders::_2;

	template <typename Key, typename Value, order_int BtreeOrder, StorePlace Place = StorePlace::Memory>
	class NodeFactory;
	// 最好把 MiddleNode 和 LeafNode 的构造与 Btree 隔绝起来，使用 NodeBase 来作用，顶多使用强制转型来调用一些函数
	template <typename Key, typename Value, order_int BtreeOrder, StorePlace Place = StorePlace::Memory>
	class MiddleNode : public NodeBase<Key, Value, BtreeOrder, Place>
	{
	private:
		template <typename... Ts>
		using Ptr = typename TypeConfig::template Ptr<Place>::template Type<Ts...>;
		friend struct FuncLib::Persistence::ByteConverter<MiddleNode, false>;
		friend struct FuncLib::Persistence::TypeConverter<MiddleNode<Key, Value, BtreeOrder, StorePlace::Memory>>;
		friend class NodeFactory<Key, Value, BtreeOrder, Place>;
		using Base1 = NodeBase<Key, Value, BtreeOrder, Place>;
		template <bool IsLeaf, typename Node, typename Item>
		friend void Base1::AddWith(typename Base1::template OwnerLessPtr<Node> previous, typename Base1::template OwnerLessPtr<Node> next, Node *self, Item p);
		template <bool IsLeaf, typename Node, typename NoWhereToProcessCallback>
		friend void Base1::AdjustAfterRemove(RAW_PTR(Node) previous, RAW_PTR(Node) next, Node* self, NoWhereToProcessCallback noWhereToProcessCallback);

		using Leaf = LeafNode<Key, Value, BtreeOrder, Place>;
		// TODO maybe below two item could be pointer, then entity stored in its' parent like Btree do
		typename Base1::UpNodeAddSubNodeCallback const _addSubNodeCallback = bind(&MiddleNode::AddSubNodeCallback, this, _1, _2);
		typename Base1::UpNodeDeleteSubNodeCallback const _deleteSubNodeCallback = bind(&MiddleNode::DeleteSubNodeCallback, this, _1);
		typename Base1::MinKeyChangeCallback const _minKeyChangeCallback = bind(&MiddleNode::SubNodeMinKeyChangeCallback, this, _1, _2);
		typename Base1::ShallowTreeCallback* _shallowTreeCallbackPtr = nullptr;
		function<RAW_PTR(MiddleNode)(MiddleNode const*)> _queryPrevious = [](auto) { return nullptr; };
		function<RAW_PTR(MiddleNode)(MiddleNode const*)> _queryNext = [](auto) { return nullptr; };
		using StoredKey = result_of_t<decltype(&Base1::MinKey)(Base1)>;
		using StoredValue = typename TypeSelector<Place, Refable::No, Ptr<Base1>>::Result;
		Elements<StoredKey, StoredValue, BtreeOrder, LessThan<Key>> _elements;

	public:
		bool Middle() const override { return true; }

		MiddleNode() : Base1(), _elements(Base1::_lessThan)
		{ }

		MiddleNode(IEnumerator<Ptr<Base1>> auto enumerator)
			: Base1(), _elements(EnumeratorPipeline<Ptr<Base1>, typename decltype(_elements)::Item, decltype(enumerator)>(enumerator, bind(&MiddleNode::ConvertPtrToKeyPtrPair, _1)), Base1::_lessThan)
		{
			SetSubNode(false);
		}

		/// Not set query next, previous and shallow tree callbacks
		MiddleNode(MiddleNode const& that)
			: MiddleNode(EnumeratorPipeline<typename decltype(that._elements)::Item const&, Ptr<Base1>, decltype(that._elements.GetEnumerator())>(that._elements.GetEnumerator(), bind(&MiddleNode::CloneSubNode, _1)))
		{ }

		MiddleNode(MiddleNode&& that) noexcept
			: Base1(move(that)),
			  _elements(move(that._elements)),
			  _queryNext(move(that._queryNext)), _queryPrevious(move(that._queryPrevious)),
			  _shallowTreeCallbackPtr(that._shallowTreeCallbackPtr)
		{
			SetSubNode(true); 
		}

		Ptr<Base1> Clone() const
		{
			// If mark copy constructor private, this method cannot compile pass
			// In make_unique internal will call MiddleNode copy constructor,
			// but it doesn't have access
			return this->CopyNode(this);
		}

		void SetShallowCallbackPointer(typename Base1::ShallowTreeCallback* shallowTreeCallbackPtr) override
		{
			_shallowTreeCallbackPtr = shallowTreeCallbackPtr;
		}

		void ResetShallowCallbackPointer() override
		{
			// Convenient for debug when has wrong behavior
			_shallowTreeCallbackPtr = nullptr;
		}

		vector<Key> LetMinLeafCollectKeys() const override
		{
			return MinSon()->LetMinLeafCollectKeys();
		}

		Ptr<Base1> HandleOverOnlySon()
		{
			Assert(_elements.Count() == 1);
			return _elements.PopOut().second;
		}

		result_of_t<decltype(&Base1::MinKey)(Base1)> MinKey() const override
		{
			return _elements[0].first;
		}

#define ARG_TYPE_IN_BASE(METHOD, IDX) typename FuncTraits<typename GetMemberFuncType<decltype(&Base1::METHOD)>::Result>::template Arg<IDX>::Type
#define SELECT_BRANCH(KEY) auto i = _elements.SelectBranch(KEY)
		bool ContainsKey(ARG_TYPE_IN_BASE(ContainsKey, 0) key) const override
		{
			SELECT_BRANCH(key);
			return _elements[i].second->ContainsKey(key);
		}

		typename Base1::StoredValue& GetValue(ARG_TYPE_IN_BASE(GetValue, 0) key) override
		{
			SELECT_BRANCH(key);
			return _elements[i].second->GetValue(key);
		}

		void ModifyValue(ARG_TYPE_IN_BASE(ModifyValue, 0) key, ARG_TYPE_IN_BASE(ModifyValue, 1) value) override
		{
			SELECT_BRANCH(key);
			_elements[i].second->ModifyValue(key, move(value));
		}

		void Add(ARG_TYPE_IN_BASE(Add, 0) p) override
		{
			SELECT_BRANCH(p.first);
			_elements[i].second->Add(move(p));
		}

		void Remove(ARG_TYPE_IN_BASE(Remove, 0) key) override
		{
			SELECT_BRANCH(key);
			_elements[i].second->Remove(key);
		}
#undef SELECT_BRANCH
#undef ARG_TYPE_IN_BASE

		vector<Key> KeysInThisNode() const override
		{
			return this->GetKeysFrom(_elements);
		}

		result_of_t<decltype (&Base1::SubNodes)(Base1)> SubNodes() const override
		{
			vector<RAW_PTR(Base1)> subs;
			subs.reserve(_elements.Count());
			for (auto& e : _elements)
			{
				subs.push_back(e.second.get());
			}

			return subs;
		}

		RecursiveGenerator<pair<typename Base1::StoredKey, typename Base1::StoredValue>*> GetStoredPairEnumerator() override
		{
			for (auto& e : _elements)
			{
				co_yield e.second->GetStoredPairEnumerator();
			}
		}
	private:
		// element LessThanPtr is not set
		MiddleNode(decltype(_elements) elements) : Base1(), _elements(move(elements), Base1::_lessThan)
		{
			SetSubNode(true);
		}

		RAW_PTR(Base1) MinSon() const { return _elements[0].second.get(); }
		RAW_PTR(Base1) MaxSon() const { return _elements[_elements.Count() - 1].second.get(); }

// TODO 注意使用下面这些转型的地方
#define MID_CAST(NODE) static_cast<RAW_PTR(MiddleNode)> (NODE)
#define LEF_CAST(NODE) static_cast<RAW_PTR(Leaf)>(NODE)
		RAW_PTR(Leaf) MinLeafInMyRange() const
		{
			if (auto node = MinSon(); node->Middle())
			{
				return LEF_CAST(MID_CAST(node)->MinLeafInMyRange());
			}
			else
			{
				return LEF_CAST(node);
			}
		}

		RAW_PTR(Leaf) MaxLeafInMyRange() const
		{
			if (auto node = MaxSon(); node->Middle())
			{
				return LEF_CAST(MID_CAST(node)->MaxLeafInMyRange());
			}
			else
			{
				return LEF_CAST(node);
			}
		}

		void AddSubNodeCallback(Base1* srcNode, Ptr<Base1> newNextNode)
		{
			// newNextNode must not be MinSon
			if (not _elements.Full())
			{
				return this->ProcessedAdd<false>({ StoredKey(newNextNode->MinKey()), move(newNextNode) });
			}

			auto next = _queryNext(this);
			auto previous = _queryPrevious(this);
			typename decltype(_elements)::Item p{ StoredKey(newNextNode->MinKey()), move(newNextNode) };
			Base1::template AddWith<false>(previous, next, this, move(p));
		}

		void DeleteSubNodeCallback(Base1* node)
		{
			// node has no key
			auto i = IndexOfSubNode(node);
			_elements.RemoveAt(i);
			if (i == 0)
			{
				(*this->_minKeyChangeCallbackPtr)(MinKey(), this);
			}

			constexpr auto lowBound = Base1::LowBound;
			if (_elements.Count() < lowBound)
			{
				auto next = _queryNext(this);
				auto previous = _queryPrevious(this);
				Base1::template AdjustAfterRemove<false>(previous, next, this, [this]{ (*this->_shallowTreeCallbackPtr)(); });
				// 上面这回调是发生在本身是 root 的那个 node
				// 加层和减层这两个操作只能发生在 root
			}
		}

		void SubNodeMinKeyChangeCallback(result_of_t<decltype(&Base1::MinKey)(Base1)> newMinKeyOfSubNode, Base1* subNode)
		{
			auto i = IndexOfSubNode(subNode);
			_elements[i].first = StoredKey(newMinKeyOfSubNode);

			if (i == 0)
			{
				(*this->_minKeyChangeCallbackPtr)(newMinKeyOfSubNode, this);
			}
		}

		RAW_PTR(MiddleNode) QuerySubNodePreviousCallback(MiddleNode const* subNode) const
		{
			if (auto i = _elements.IndexKeyOf(subNode->MinKey()); i == 0)
			{
				if (RAW_PTR(MiddleNode) pre = _queryPrevious(this); pre != nullptr)
				{
					return MID_CAST(pre->MaxSon());
				}

				return nullptr;
			}
			else
			{
				return MID_CAST(_elements[i - 1].second.get());
			}
		}

		RAW_PTR(MiddleNode) QuerySubNodeNextCallback(MiddleNode const* subNode) const
		{
			if (auto i = _elements.IndexKeyOf(subNode->MinKey()); i == _elements.Count() - 1)
			{
				if (RAW_PTR(MiddleNode) next = _queryNext(this); next != nullptr)
				{
					return MID_CAST(next->MinSon());
				}

				return nullptr;
			} 
			else
			{
				return MID_CAST(_elements[i + 1].second.get());
			}
		}

		void SetSubNode(bool leafRelationCorrect)
		{
			if (_elements.Empty())
			{
				return;
			}

			RAW_PTR(Leaf) lastLeaf = nullptr;
			MiddleNode* lastMidNode = nullptr;
			bool subIsMiddle = MinSon()->Middle();
			for (auto& e : _elements)
			{
				auto& node = e.second;
				SetSubNodeCallback(subIsMiddle, node);

				if (leafRelationCorrect)
				{
					continue;
				}

				// set leaf relation
				if (subIsMiddle)
				{
					// set previous and next LeafNode between MiddleNode
					auto midNode = MID_CAST(node.get()); // 这里转型后直接读取会有问题 TODO 想改成 set property 回调里读后再转型
					if (lastMidNode != nullptr)
					{
						auto nowMin = midNode->MinLeafInMyRange();
						auto lastMax = lastMidNode->MaxLeafInMyRange();
						SET_PROPERTY(lastMax, =, ->Next(nowMin));
						SET_PROPERTY(nowMin, =, ->Previous(lastMax));
					}

					lastMidNode = midNode;
				}
				else
				{
					// set previous and next LeafNode that contained in MiddleNode
					auto nowLeaf = LEF_CAST(node.get());
					if (lastLeaf != nullptr)
					{
						auto last = lastLeaf;// 直接用显式声明类型的 lastLeaf 不行，疑似是 clang 的 bug。
						SET_PROPERTY(nowLeaf, =, ->Previous(lastLeaf));
						SET_PROPERTY(last, =, ->Next(nowLeaf));
					}

					lastLeaf = nowLeaf;
				}
			}
		}

		void SetSubNodeCallback(bool middle, Ptr<Base1>& node)
		{
			SET_PROPERTY(node, &, ->SetUpNodeCallback(&_addSubNodeCallback, &_deleteSubNodeCallback, &_minKeyChangeCallback));

			if (middle)
			{
				auto midNode = MID_CAST(node.get());// 这个 get 可以去掉，直接 cast 吗？
				SET_PROPERTY(midNode, this, ->_queryPrevious = bind(&MiddleNode::QuerySubNodePreviousCallback, this, _1));
				SET_PROPERTY(midNode, this, ->_queryNext = bind(&MiddleNode::QuerySubNodeNextCallback, this, _1));
			}
		}
#undef MID_CAST		
#undef LEF_CAST
		// 这里的参数类型可以改一下，不直接用裸指针
		order_int IndexOfSubNode(Base1 const* node) const
		{
			auto e = _elements.GetEnumerator();
			while (e.MoveNext())
			{
				if (e.Current().second.get() == node)
				{
					return (order_int)e.CurrentIndex();
				}
			}

			throw InvalidOperationException("Cannot find node in MiddleNode::_elements");
		}

		// Below methods for same node internal use
		void AppendItems(vector<typename decltype(_elements)::Item> items)
		{
			for (auto& i : items)
			{
				Append(move(i));
			}
		}

		void Append(typename decltype(_elements)::Item item)
		{
			this->SetSubNodeCallback(item.second->Middle(), item.second);
			_elements.Append(move(item));
		}

		void EmplaceHead(typename decltype(_elements)::Item item)
		{
			this->SetSubNodeCallback(item.second->Middle(), item.second);
			_elements.EmplaceHead(move(item));
			(*this->_minKeyChangeCallbackPtr)(this->MinKey(), this);
		}

		template <bool SelfIsNewNode>
		void ProcessedAdd(typename decltype(_elements)::Item item)
		{
			this->SetSubNodeCallback(item.second->Middle(), item.second);
			if constexpr (SelfIsNewNode)
			{
				_elements.Add(move(item));
			}
			else
			{
				_elements.Add(move(item), [this]()
				{
					(*this->_minKeyChangeCallbackPtr)(this->MinKey(), this);
				});
			}
		}

		typename decltype(_elements)::Item
		ExchangeMin(typename decltype(_elements)::Item item)
		{
			this->SetSubNodeCallback(item.second->Middle(), item.second);
			auto min = _elements.ExchangeMin(move(item));
			(*this->_minKeyChangeCallbackPtr)(this->MinKey(), this);
			return move(min);
		}

		typename decltype(_elements)::Item
		ExchangeMax(typename decltype(_elements)::Item item)
		{
			this->SetSubNodeCallback(item.second->Middle(), item.second);
			return _elements.ExchangeMax(move(item));
		}

		static typename decltype(_elements)::Item ConvertRefPtrToKeyPtrPair(Ptr<Base1>& node)
		{
			using pairType = typename decltype(_elements)::Item;
			return make_pair<typename pairType::first_type, typename pairType::second_type>(StoredKey(node->MinKey()), move(node));
		}

		static typename decltype(_elements)::Item ConvertPtrToKeyPtrPair(Ptr<Base1> node)
		{
			using pairType = typename decltype(_elements)::Item;
			return make_pair<typename pairType::first_type, typename pairType::second_type>(StoredKey(node->MinKey()), move(node));
		}

		static Ptr<Base1> CloneSubNode(typename decltype(_elements)::Item const& item)
		{
			return Collections::Clone(item.second.get());
		}
	};
}