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
			: Base(), _elements(EnumeratorPipeline<unique_ptr<Base>&, typename decltype(_elements)::Item>(enumerator, bind(&MiddleNode::ConvertRefPtrToKeyPtrPair, _1)), lessThanPtr)
		{
			SetSubNode();
		}

		MiddleNode(MiddleNode const& that)
			: MiddleNode(EnumeratorPipeline<typename decltype(that._elements)::Item const&, unique_ptr<Base>>(that._elements.GetEnumerator(), bind(&MiddleNode::CloneSubNode, _1)), that._elements.LessThanPtr)
		{ }

		MiddleNode(MiddleNode&& that) noexcept : Base(move(that)), _elements(move(that._elements))
		{ }

		~MiddleNode() override = default;

		unique_ptr<Base> Clone() const override
		{
			return make_unique<MiddleNode>(*this);
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

#define MID_CAST(NODE) static_cast<MiddleNode *>(NODE)
		MiddleNode* Next() const
		{
			return MID_CAST(Base::Next());
		}

		MiddleNode* Previous() const
		{
			return MID_CAST(Base::Previous());
		}
	private:
		MiddleNode(IEnumerator<unique_ptr<Base>>&& enumerator, shared_ptr<_LessThan> lessThanPtr)
			: Base(), _elements(EnumeratorPipeline<unique_ptr<Base>, typename decltype(_elements)::Item>(enumerator, bind(&MiddleNode::ConvertPtrToKeyPtrPair, _1)), lessThanPtr)
		{
			SetSubNode();
		}

		Base* MinSon() const { return _elements[0].second.get(); }
		Base* MaxSon() const { return _elements[_elements.Count() - 1].second.get(); }
		
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
			// find index of srcNode and add new NextNode
			// if this is Full(), combine the node or call the upper node callback
			if (!_elements.Full())
			{
				_elements.Emplace(_elements.IndexKeyOf(srcNode->MinKey()), { cref(newNextNode->MinKey()), move(newNextNode) });
				return;
			}

			auto _next = this->Next();
			auto _previous = this->Previous();
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
			auto _next = this->Next();
			auto _previous = this->Previous();
			AFTER_REMOVE_COMMON;

			// Update min key
			// Below code should be moved into AFTER_REMOVE_COMMON
			if (i == 0)
			{
				_elements[0].first = cref(MinKey());
			}
		}

		void SubNodeMinKeyCallback(Key const& newMinKeyOfSubNode, NodeBase* subNode)
		{
			// Index of subNode
			auto i;
			_elements[i].first = cref(minKeyOfSubNode);

			if (i == 0)
			{
				Base::_minKeyChangeCallback(minKeyOfSubNode, this);
			}
		}

		void SetSubNode()
		{
			using ::std::placeholders::_1;
			using ::std::placeholders::_2;
			Leaf* lastLeaf = nullptr;
			MiddleNode* lastMidNode = nullptr;
			for (auto& e : _elements)
			{
				auto& node = e.second;
				node->SetUpNodeCallback(bind(&MiddleNode::AddSubNodeCallback, this, _1, _2),
					 bind(&MiddleNode::DeleteSubNodeCallback, this, _1),
					 bind(&MiddleNode::SubNodeMinKeyCallback, this, _1, _2));

				if (lastMidNode != nullptr || node->Middle())
				{
					auto midNode = MID_CAST(node.get());

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
					nowLeaf->Previous(lastLeaf);
					if (lastLeaf != nullptr)
					{
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