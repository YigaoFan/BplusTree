#pragma once
#include <memory>
#include <functional>
#include <type_traits>
#include "../Basic/Exception.hpp"
#include "../Basic/TypeTrait.hpp"
#include "Basic.hpp"
#include "EnumeratorPipeline.hpp"
#include "Elements.hpp"
#include "NodeBase.hpp"
#include "LeafNode.hpp"
#include "../FuncLib/FriendFuncLibDeclare.hpp"
#include "../FuncLib/PtrSetter.hpp"
#include "NodeAddRemoveCommon.hpp"

namespace Collections 
{
	using ::std::move;
	using ::std::unique_ptr;
	using ::std::reference_wrapper;
	using ::std::pair;
	using ::std::make_pair;
	using ::std::bind;
	using ::std::placeholders::_1;
	using ::std::placeholders::_2;
	using ::Basic::NotImplementException;
	using ::Basic::CompileIf;
	using ::Basic::IsSpecialization;// PtrSetter use
	using ::std::is_fundamental_v;

	template <typename Key, typename Value, order_int BtreeOrder, template <typename...> class Ptr>
	class NodeFactory;
	// 最好把 MiddleNode 和 LeafNode 的构造与 Btree 隔绝起来，使用 NodeBase 来作用，顶多使用强制转型来调用一些函数
	template <typename Key, typename Value, order_int BtreeOrder, template <typename...> class Ptr = unique_ptr>
	class MiddleNode : public NodeBase<Key, Value, BtreeOrder, Ptr>
	{
	private:
		friend struct FuncLib::ByteConverter<MiddleNode, false>;
		friend struct FuncLib::TypeConverter<MiddleNode, false>;
		friend class NodeFactory<Key, Value, BtreeOrder, Ptr>;
		using Base = NodeBase<Key, Value, BtreeOrder, Ptr>;
		using Leaf = LeafNode<Key, Value, BtreeOrder, Ptr>;
		using StoredKey = typename CompileIf<is_fundamental_v<Key>, Key, reference_wrapper<Key const>>::Type;
		// TODO maybe below two item could be pointer, then entity stored in its' parent like Btree do
		typename Base::UpNodeAddSubNodeCallback _addSubNodeCallback = bind(&MiddleNode::AddSubNodeCallback, this, _1, _2);
		typename Base::UpNodeDeleteSubNodeCallback _deleteSubNodeCallback = bind(&MiddleNode::DeleteSubNodeCallback, this, _1);
		typename Base::MinKeyChangeCallback _minKeyChangeCallback = bind(&MiddleNode::SubNodeMinKeyChangeCallback, this, _1, _2);
		typename Base::ShallowTreeCallback* _shallowTreeCallbackPtr = nullptr;
		function<MiddleNode *(MiddleNode const*)> _queryPrevious = [](auto) { return nullptr; };
		function<MiddleNode *(MiddleNode const*)> _queryNext = [](auto) { return nullptr; };
		using _LessThan = LessThan<Key>;
		Elements<StoredKey, Ptr<Base>, BtreeOrder, _LessThan> _elements;

	public:
		bool Middle() const override { return true; }

		MiddleNode(shared_ptr<_LessThan> lessThanPtr)
			: Base(), _elements(lessThanPtr)
		{ }	

		MiddleNode(IEnumerator<Ptr<Base>&>& enumerator, shared_ptr<_LessThan> lessThanPtr)
			: Base(), _elements(EnumeratorPipeline<Ptr<Base>&, typename decltype(_elements)::Item>(enumerator, bind(&MiddleNode::ConvertRefPtrToKeyPtrPair, _1)), lessThanPtr)
		{
			SetSubNode();
		}

		MiddleNode(MiddleNode const& that)
			: MiddleNode(EnumeratorPipeline<typename decltype(that._elements)::Item const&, Ptr<Base>>(that._elements.GetEnumerator(), bind(&MiddleNode::CloneSubNode, _1)), that._elements.LessThanPtr)
		{ }

		MiddleNode(MiddleNode&& that) noexcept
			: Base(move(that)), _elements(move(that._elements)), 
			_queryNext(move(that._queryNext)), _queryPrevious(move(that._queryPrevious)),
			_shallowTreeCallbackPtr(that._shallowTreeCallbackPtr)
		{ }

		~MiddleNode() override = default;

		Ptr<Base> Clone() const override
		{
			// If mark copy constructor private, this method cannot compile pass
			// In make_unique internal will call MiddleNode copy constructor,
			// but it doesn't have access
			return make_unique<MiddleNode>(*this);
		}

		DEF_LESS_THAN_SETTER

		void SetShallowCallbackPointer(typename Base::ShallowTreeCallback* shallowTreeCallbackPtr) override
		{
			_shallowTreeCallbackPtr = shallowTreeCallbackPtr;
		}

		void ResetShallowCallbackPointer() override
		{
			// Convenient for debug when has wrong behavior
			_shallowTreeCallbackPtr = nullptr;
		}

		vector<Key> Keys() const override
		{
			return MinSon()->Keys();
		}

		Ptr<Base> HandleOverOnlySon()
		{
#ifdef BTREE_DEBUG 
			Assert(_elements.Count() == 1); // TODO implement Assert
#endif
			return _elements.PopOut().second;
		}

		Key const& MinKey() const override
		{
			return _elements[0].first;
		}

#define SELECT_BRANCH(KEY) auto i = _elements.SelectBranch(KEY)
		bool ContainsKey(Key const& key) const override
		{
			SELECT_BRANCH(key);
			return _elements[i].second->ContainsKey(key);
		}

		Value GetValue(Key const& key) const override
		{
			SELECT_BRANCH(key);
			return _elements[i].second->GetValue(key);
		}

		void ModifyValue(Key const& key, Value value) override
		{
			SELECT_BRANCH(key);
			_elements[i].second->ModifyValue(key, move(value));
		}

		void Add(pair<Key, Value> p) override
		{
			SELECT_BRANCH(p.first);
			_elements[i].second->Add(move(p));
			// TODO why up code prefer global []
		}

		void Remove(Key const& key) override
		{
			SELECT_BRANCH(key);
			_elements[i].second->Remove(key);
		}
#undef SELECT_BRANCH

		vector<Key> SubNodeMinKeys() const override
		{
			vector<Key> ks;
			ks.reserve(_elements.Count());
			for (auto k : _elements.Keys())
			{
				ks.push_back(k);
			}

			return ks;
		}

		vector<Base*> SubNodes() const override
		{
			vector<Base*> subs;
			subs.reserve(_elements.Count());
			for (auto& e : _elements)
			{
				subs.push_back(e.second.get());
			}

			return subs;
		}
	private:
		MiddleNode(decltype(_elements) elements) : Base(), _elements(move(elements))// TODO less than?
		{
			SetSubNode();
		}

		MiddleNode(IEnumerator<Ptr<Base>>&& enumerator, shared_ptr<_LessThan> lessThanPtr)
			: Base(), _elements(EnumeratorPipeline<Ptr<Base>, typename decltype(_elements)::Item>(enumerator, bind(&MiddleNode::ConvertPtrToKeyPtrPair, _1)), lessThanPtr)
		{
			SetSubNode();
		}

		Base* MinSon() const { return _elements[0].second.get(); }
		Base* MaxSon() const { return _elements[_elements.Count() - 1].second.get(); }
		
#define MID_CAST(NODE) static_cast<MiddleNode *>(NODE)
#define LEF_CAST(NODE) static_cast<Leaf *>(NODE)
		Leaf* MinLeafInMyRange() const
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

		Leaf* MaxLeafInMyRange() const
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

		void AddSubNodeCallback(Base* srcNode, Ptr<Base> newNextNode)
		{
			// newNextNode must not be MinSon
			if (!_elements.Full())
			{
				return this->ProcessedAdd({ StoredKey(newNextNode->MinKey()), move(newNextNode) });
			}

			auto next = _queryNext(this);
			auto previous = _queryPrevious(this);
			typename decltype(_elements)::Item p{ StoredKey(newNextNode->MinKey()), move(newNextNode) };
			ADD_COMMON(false);
		}

		void DeleteSubNodeCallback(Base* node)
		{
			// node has no key
			auto i = IndexOfSubNode(node);
			_elements.RemoveAt(i);
			if (i == 0)
			{
				(*this->_minKeyChangeCallbackPtr)(MinKey(), this);
			}

			constexpr auto lowBound = Base::LowBound;
			if (_elements.Count() < lowBound)
			{
				// Below two variables is to macro
				auto next = _queryNext(this);
				auto previous = _queryPrevious(this);
				AFTER_REMOVE_COMMON(false);
				// MiddleNode need to handle NoWhereToProcess
				// 下面这句是发生在 root 那个 node
				// 加层和减层这两个操作只能发生在 root
				// 所以下面这句在普通 MiddleNode 发生不了
				// 下面这句如何确保是 root 节点调用的呢？
				(*this->_shallowTreeCallbackPtr)();
			}
		}

		void SubNodeMinKeyChangeCallback(Key const& newMinKeyOfSubNode, Base* subNode)
		{
			auto i = IndexOfSubNode(subNode);
			_elements[i].first = StoredKey(newMinKeyOfSubNode);

			if (i == 0)
			{
				(*this->_minKeyChangeCallbackPtr)(newMinKeyOfSubNode, this);
			}
		}

		MiddleNode* QuerySubNodePreviousCallback(MiddleNode const* subNode) const
		{
			if (auto i = _elements.IndexKeyOf(subNode->MinKey()); i == 0)
			{
				if (MiddleNode* pre = _queryPrevious(this); pre != nullptr)
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

		MiddleNode* QuerySubNodeNextCallback(MiddleNode const* subNode) const
		{
			if (auto i = _elements.IndexKeyOf(subNode->MinKey()); i == _elements.Count() - 1)
			{
				if (MiddleNode* next = _queryNext(this); next != nullptr)
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

		void SetSubNode()
		{
			// TODO use SetProperty below
			Leaf* lastLeaf = nullptr;
			MiddleNode* lastMidNode = nullptr;
			bool subIsMiddle = MinSon()->Middle();
			for (auto& e : _elements)
			{
				auto& node = e.second;
				SetSubNodeCallback(subIsMiddle, node);

				if (subIsMiddle)
				{
					// TODO type cast
					auto midNode = MID_CAST(node.get());
					// set previous and next between MiddleNode
					if (lastMidNode != nullptr)
					{
						auto nowMin = midNode->MinLeafInMyRange();
						auto lastMax = lastMidNode->MaxLeafInMyRange();
						SET_PROPERTY(lastMax, ->Next(nowMin));// TODO newNextNode should also set in MiddleNode? not in itself
						SET_PROPERTY(nowMin, ->Previous(lastMax));
					}

					lastMidNode = midNode;
				}
				else
				{
					// set previous and next in MiddleNode that contains Leafnode internal
					auto nowLeaf = LEF_CAST(node.get());
					if (lastLeaf != nullptr)
					{
						auto last = lastLeaf;// 直接用显式声明类型的 lastLeaf 不行，疑似是 clang 的 bug。
						SET_PROPERTY(nowLeaf, ->Previous(lastLeaf));// TODO newNextNode should also set in MiddleNode? not in itself
						SET_PROPERTY(last, ->Next(nowLeaf));
					}

					lastLeaf = nowLeaf;
				}
			}
		}

		void SetSubNodeCallback(bool middle, Ptr<Base> const& node)
		{
			SET_PROPERTY(node, ->SetUpNodeCallback(&_addSubNodeCallback, &_deleteSubNodeCallback, &_minKeyChangeCallback));

			if (middle)
			{
				// TODO type cast
				auto midNode = MID_CAST(node.get());
				SET_PROPERTY(midNode, ->_queryPrevious = bind(&MiddleNode::QuerySubNodePreviousCallback, this, _1));
				SET_PROPERTY(midNode, ->_queryNext = bind(&MiddleNode::QuerySubNodeNextCallback, this, _1));
			}
		}
#undef MID_CAST		
#undef LEF_CAST
		order_int IndexOfSubNode(Base const* node) const
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

		void ProcessedAdd(typename decltype(_elements)::Item item)
		{
			this->SetSubNodeCallback(item.second->Middle(), item.second);
			_elements.Add(move(item), [this]()
			{
				(*this->_minKeyChangeCallbackPtr)(this->MinKey(), this);
			});
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

		static typename decltype(_elements)::Item ConvertRefPtrToKeyPtrPair(Ptr<Base>& node)
		{
			using pairType = typename decltype(_elements)::Item;
			return make_pair<typename pairType::first_type, typename pairType::second_type>(StoredKey(node->MinKey()), move(node));
		}

		static typename decltype(_elements)::Item ConvertPtrToKeyPtrPair(Ptr<Base> node)
		{
			using pairType = typename decltype(_elements)::Item;
			return make_pair<typename pairType::first_type, typename pairType::second_type>(StoredKey(node->MinKey()), move(node));
		}

		static Ptr<Base> CloneSubNode(typename decltype(_elements)::Item const& item)
		{
			return item.second->Clone();
		}
	};
}