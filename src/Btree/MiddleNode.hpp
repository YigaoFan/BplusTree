#pragma once
#include <memory>
#include <functional>
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

	template <typename Key, typename Value, order_int BtreeOrder>
	class MiddleNode : public NodeBase<Key, Value, BtreeOrder>
	{
	private:
		function<MiddleNode*(MiddleNode*)> _queryPrevious;
		function<MiddleNode*(MiddleNode*)> _queryNext;
		using Base = NodeBase<Key, Value, BtreeOrder>;
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
			: Base(), _elements(EnumeratorPipeline<unique_ptr<Base>&, typename decltype(_elements)::Item>(enumerator, bind(&MiddleNode::ConvertToKeyBasePtrPair, _1)), lessThanPtr)
		{
			SetSubNode();
		}

		MiddleNode(MiddleNode const& that)
			: MiddleNode(EnumeratorPipeline<typename decltype(that._elements)::Item&, unique_ptr<Base>>(that._elements.GetEnumerator(), bind(&MiddleNode::CloneSubNode, _1)), that._elements.LessThanPtr)
		{ }

		MiddleNode(MiddleNode&& that) noexcept : Base(move(that)), _elements(move(that._elements))
		{ }

		~MiddleNode() override = default;

		unique_ptr<Base> Clone() const override
		{
			// _queryPrevious how to process
			return make_unique<MiddleNode>(*this);
			// auto cloneOne = make_unique<MiddleNode>(_elements.LessThanPtr);
			// for (auto const& e : _elements)
			// {
			// 	auto subCloned = e.second->Clone();
			// 	cloneOne->_elements.Append(ConvertToKeyBasePtrPair(subCloned));
			// }

			// return move(cloneOne);
		}

		// TODO wait to delete
		decltype(_elements) CloneElements() const
		{
			decltype(_elements) thatElements(_elements.LessThanPtr);
			for (auto const& e : _elements)
			{
				auto ptr = e.second->Clone();
				// TODO code below maybe compile error
				thatElements.Add(cref(ptr->Minkey()), move(ptr));
			}

			return move(thatElements);
		}

		vector<Key> Keys() const override
		{
			return MinSon()->Keys();
		}

		unique_ptr<Base> HandleOverOnlySon()
		{
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
			: Base(), _elements(EnumeratorPipeline<unique_ptr<Base>, typename decltype(_elements)::Item>(enumerator, bind(&MiddleNode::ConvertToKeyBasePtrPair, _1)), lessThanPtr)
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
			//auto predicate = [srcNode = srcNode]((typename (decltype _elements)::Item const&) item)
			// TODO why up is a compile error
			// auto predicate = [srcNode = srcNode](auto& item)
			// {
			// 	if (item.second.get() == srcNode)
			// 	{
			// 		return true;
			// 	}

			// 	return false;
			// };
			
			// find index of srcNode and add new NextNode
			// if this is Full(), combine the node or call the upper node callback
			if (!_elements.Full())
			{
				_elements.Emplace(_elements.IndexKeyOf(srcNode->MinKey()), { cref(newNextNode->MinKey()), move(newNextNode) });
				return;
			}

			auto _next = _queryNext(this);
			auto _previous = _queryPrevious(this);
			typename decltype(_elements)::Item p{ cref(newNextNode->MinKey()), move(newNextNode) };
			ADD_COMMON(false);
		}


		void DeleteSubNodeCallback(Base* node)
		{
			auto i = _elements.IndexKeyOf(node->MinKey());
			if (!node->Middle())
			{
				auto leafNode = LEF_CAST(node);
				// Fresh the _next of LeafNode
				if (i != 0)
				{
					LEF_CAST(_elements[i - 1].second.get())->NextLeaf(leafNode->NextLeaf());
				}

				// Fresh the _previous of LeafNode
				if (i != _elements.Count() - 1)
				{
					LEF_CAST(_elements[i + 1].second.get())->PreviousLeaf(leafNode->PreviousLeaf());
				}
			}

			_elements.RemoveAt(i);
			// Below two variables is to macro
			auto _next = _queryNext(this);
			auto _previous = _queryPrevious(this);
			REMOVE_COMMON;
		}

		// For sub node call
		MiddleNode* QueryPrevious(MiddleNode* subNode)
		{
			auto i = _elements.IndexKeyOf(subNode->MinKey());
			return i != 0 ? MID_CAST(_elements[i - 1].second.get()) : nullptr;
		}

		// For sub node call
		MiddleNode* QueryNext(MiddleNode* subNode)
		{
			auto i = _elements.IndexKeyOf(subNode->MinKey());
			return (i != _elements.Count() - 1) ? MID_CAST(_elements[i + 1].second.get()) : nullptr;
		}

		void SetSubNode()
		{
			using ::std::placeholders::_1;
			using ::std::placeholders::_2;
			auto f1 = bind(&MiddleNode::AddSubNodeCallback, this, _1, _2);
			auto f2 = bind(&MiddleNode::DeleteSubNodeCallback, this, _1);
			Leaf* lastLeaf = nullptr;
			MiddleNode* lastMidNode = nullptr;
			for (auto& e : _elements)
			{
				auto& node = e.second;
				node->SetUpNodeCallback(f1, f2);
				if (node->Middle())
				{
					auto midNode = MID_CAST(node.get());
					midNode->_queryNext = bind(&MiddleNode::QueryNext, this, _1);
					midNode->_queryPrevious = bind(&MiddleNode::QueryPrevious, this, _1);

					// set preivous and next between MiddleNode
					if (lastMidNode != nullptr)
					{
						auto nowMin = midNode->MinLeafInMyRange();
						auto lastMax = lastMidNode->MaxLeafInMyRange();
						lastMax->NextLeaf(nowMin);
						nowMin->PreviousLeaf(lastMax);
					}

					lastMidNode = midNode;
				}
				else
				{
					// set preivous and next in MiddleNode internal
					auto nowLeaf = LEF_CAST(node.get());
					nowLeaf->PreviousLeaf(lastLeaf);
					if (lastLeaf != nullptr)
					{
						lastLeaf->NextLeaf(nowLeaf);
					}

					lastLeaf = nowLeaf;
				}
				
			}
		}
#undef MID_CAST		
#undef LEF_CAST

		static typename decltype(_elements)::Item ConvertToKeyBasePtrPair(unique_ptr<Base>& node)
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