#pragma once
#include <memory>
#include <functional>
#include <type_traits>
#include "../Basic/Exception.hpp"
#include "Basic.hpp"
#include "EnumeratorPipeline.hpp"
#include "Elements.hpp"
#include "NodeBase.hpp"
#include "LeafNode.hpp"
#include "NodeAddRemoveCommon.hpp"

namespace Collections 
{
	using ::std::move;
	using ::std::unique_ptr;
	using ::std::cref;
	using ::std::reference_wrapper;
	using ::std::pair;
	using ::std::make_pair;
	using ::std::bind;
	using ::std::placeholders::_1;
	using ::Basic::NotImplementException;
	using ::std::add_const;

	template <typename Key, typename Value, order_int BtreeOrder>
	class NodeFactory;
	// 最好把 MiddleNode 和 LeafNode 的构造与 Btree 隔绝起来，使用 NodeBase 来作用，顶多使用强制转型来调用一些函数
	template <typename Key, typename Value, order_int BtreeOrder>
	class MiddleNode : public NodeBase<Key, Value, BtreeOrder>
	{
	private:
		friend class NodeFactory<Key, Value, BtreeOrder>;
		using Base = NodeBase<Key, Value, BtreeOrder>;
		// TODO Split node need to copy two below items
		// TODO maybe below two item could be pointer, then entity stored in its' parent like Btree do
		function<MiddleNode *(MiddleNode const*)> _queryPrevious = [](auto) { return nullptr; };
		function<MiddleNode *(MiddleNode const*)> _queryNext = [](auto) { return nullptr; };
		function<void()> * _shallowTreeCallbackPtr = nullptr;
		using _LessThan = LessThan<Key>;
		Elements<reference_wrapper<Key const>, unique_ptr<Base>, BtreeOrder, _LessThan> _elements;

	public:
		bool Middle() const override { return true; }

		MiddleNode(shared_ptr<_LessThan> lessThanPtr)
			: Base(), _elements(lessThanPtr)
		{
			SetSubNode();
		}	

		MiddleNode(IEnumerator<unique_ptr<Base>&>& enumerator, shared_ptr<_LessThan> lessThanPtr)
			: Base(), _elements(EnumeratorPipeline<unique_ptr<Base>&, typename decltype(_elements)::Item>(enumerator, bind(&MiddleNode::ConvertRefPtrToKeyPtrPair, _1)), lessThanPtr)
		{
			SetSubNode();
		}

		// TODO private this method?
		MiddleNode(MiddleNode const& that)
			: MiddleNode(EnumeratorPipeline<typename decltype(that._elements)::Item const&, unique_ptr<Base>>(that._elements.GetEnumerator(), bind(&MiddleNode::CloneSubNode, _1)), that._elements.LessThanPtr)
		{ }

		MiddleNode(MiddleNode&& that) noexcept
			: Base(move(that)), _elements(move(that._elements)), 
			_queryNext(move(that._queryNext)), _queryPrevious(move(that._queryPrevious)),
			_shallowTreeCallbackPtr(that._shallowTreeCallbackPtr)
		{ }

		~MiddleNode() override = default;

		unique_ptr<Base> Clone() const override
		{
			return make_unique<MiddleNode>(*this);
		}

		void SetShallowCallbackPointer(function<void()>* shallowTreeCallbackPtr) override
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

		unique_ptr<Base> HandleOverOnlySon()
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

	private:
		MiddleNode(IEnumerator<unique_ptr<Base>>&& enumerator, shared_ptr<_LessThan> lessThanPtr)
			: Base(), _elements(EnumeratorPipeline<unique_ptr<Base>, typename decltype(_elements)::Item>(enumerator, bind(&MiddleNode::ConvertPtrToKeyPtrPair, _1)), lessThanPtr)
		{
			SetSubNode();
		}

		Base* MinSon() const { return _elements[0].second.get(); }
		Base* MaxSon() const { return _elements[_elements.Count() - 1].second.get(); }
		
#define MID_CAST(NODE) static_cast<MiddleNode *>(NODE)
		using Leaf = LeafNode<Key, Value, BtreeOrder>;
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

		void AddSubNodeCallback(Base* srcNode, unique_ptr<Base> newNextNode)
		{
			if (!_elements.Full())
			{
				_elements.Emplace(_elements.IndexKeyOf(srcNode->MinKey()) + 1, { cref(newNextNode->MinKey()), move(newNextNode) });
				return;
			}

			auto _next = _queryNext(this);
			auto _previous = _queryPrevious(this);
			typename decltype(_elements)::Item p{ cref(newNextNode->MinKey()), move(newNextNode) };
			ADD_COMMON(false);
		}

		void DeleteSubNodeCallback(Base* node)// TODO this node cannot be MinSon?
		{
			auto i = _elements.IndexKeyOf(node->MinKey());
			if (!node->Middle())
			{
				auto leafNode = LEF_CAST(node);
				// Fresh the _next of LeafNode
				if (i != 0)
				{
					LEF_CAST(_elements[i - 1].second.get())->Next(leafNode->Next());
				}

				// Fresh the _previous of LeafNode
				if (i != _elements.Count() - 1)
				{
					LEF_CAST(_elements[i + 1].second.get())->Previous(leafNode->Previous());
				}
			}

			_elements.RemoveAt(i);
			// Below two variables is to macro
			auto _next = _queryNext(this);
			auto _previous = _queryPrevious(this);
			AFTER_REMOVE_COMMON(false);
			// MiddleNode need to handle NoWhereToProcess
			// 下面这句是发生在 root 那个 node
			// 加层和减层这两个操作只能发生在 root
			// 所以下面这句在普通 MiddleNode 发生不了
			// 下面这句如何确保是 root 节点调用的呢？
			this->_shallowTreeCallbackPtr->operator ()();
		}

		void SubNodeMinKeyChangeCallback(Key const& newMinKeyOfSubNode, Base* subNode)
		{
			auto indexOfSubNode = [this](Base* node)
			{
				auto e = this->_elements.GetEnumerator();
				while (e.MoveNext())
				{
					if (e.Current().second.get() == node)
					{
						return e.CurrentIndex();
					}
				}

				throw InvalidOperationException("Cannot find node in MiddleNode::_elements");
			};
			auto i = (order_int)indexOfSubNode(subNode);
			_elements[i].first = cref(newMinKeyOfSubNode);

			if (i == 0)
			{
				this->_minKeyChangeCallback(newMinKeyOfSubNode, this);
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
			using ::std::placeholders::_2;
			Leaf* lastLeaf = nullptr;
			MiddleNode* lastMidNode = nullptr;
			bool subIsMiddle = MinSon()->Middle();
			for (auto& e : _elements)
			{
				auto& node = e.second;
				node->SetUpNodeCallback(bind(&MiddleNode::AddSubNodeCallback, this, _1, _2),
					 bind(&MiddleNode::DeleteSubNodeCallback, this, _1),
					 bind(&MiddleNode::SubNodeMinKeyChangeCallback, this, _1, _2));

				if (subIsMiddle)
				{
					auto midNode = MID_CAST(node.get());
					midNode->_queryPrevious = bind(&MiddleNode::QuerySubNodePreviousCallback, this, _1);
					midNode->_queryNext = bind(&MiddleNode::QuerySubNodeNextCallback, this, _1);

					// set previous and next between MiddleNode
					if (lastMidNode != nullptr)
					{
						auto nowMin = midNode->MinLeafInMyRange();
						auto lastMax = lastMidNode->MaxLeafInMyRange();
						lastMax->Next(nowMin);
						nowMin->Previous(lastMax);
					}

					lastMidNode = midNode;
				}
				else
				{
					// set previous and next in MiddleNode that contains Leafnode internal
					auto nowLeaf = LEF_CAST(node.get());
					if (lastLeaf != nullptr)
					{
						nowLeaf->Previous(lastLeaf);
						lastLeaf->Next(nowLeaf);
					}

					lastLeaf = nowLeaf;
				}
			}
		}
#undef MID_CAST		
#undef LEF_CAST

		static typename decltype(_elements)::Item ConvertRefPtrToKeyPtrPair(unique_ptr<Base>& node)
		{
			using pairType = typename decltype(_elements)::Item;
			return make_pair<pairType::first_type, pairType::second_type>(cref(node->MinKey()), move(node));
		}

		static typename decltype(_elements)::Item ConvertPtrToKeyPtrPair(unique_ptr<Base> node)
		{
			using pairType = typename decltype(_elements)::Item;
			return make_pair<pairType::first_type, pairType::second_type>(cref(node->MinKey()), move(node));
		}

		static unique_ptr<Base> CloneSubNode(typename decltype(_elements)::Item const& item)
		{
			return item.second->Clone();
		}
	};
}