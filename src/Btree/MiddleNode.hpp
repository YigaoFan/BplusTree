#pragma once
#include <memory>
#include <functional>
#include "Basic.hpp"
#include "EnumeratorPipeline.hpp"
#include "Elements.hpp"
#include "NodeBase.hpp"
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
	using ::std::function;
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
		MiddleNode(shared_ptr<_LessThan> lessThanPtr)
			: Base(), _elements(lessThanPtr)
		{
			SetSubNodeCallback();
		}	

		template <typename Iterator>
		MiddleNode(Enumerator<unique_ptr<Base>&, Iterator> enumerator, shared_ptr<_LessThan> lessThanPtr)
			: Base(), _elements(EnumeratorPipeline<unique_ptr<Base>&, typename decltype(_elements)::Item>(enumerator, bind(&MiddleNode::ConvertToKeyBasePtrPair, _1)), lessThanPtr)
		{
			SetSubNodeCallback();
		}

		MiddleNode(MiddleNode& that)
			: Base(that), _elements()
		{ }

		MiddleNode(MiddleNode&& that) noexcept
			: Base(move(that)), _elements(move(that._elements))
		{ }

		~MiddleNode() override = default;

		unique_ptr<Base> Clone() override // can not be marked const, because shared_ptr will be changed if shared
		{
			auto cloneOne = make_unique<MiddleNode>(_elements.LessThanPtr);
			for (auto const& e : _elements)
			{
				auto subCloned = e.second->Clone();
				cloneOne->_elements.Append(ConvertToKeyBasePtrPair(subCloned));
			}

			return cloneOne;
		}

		decltype(_elements) CloneElements()
		{
			decltype(_elements) cloneElements;
			order_int i = 0;
			for (auto const& e : _elements)
			{
				cloneElements[i].first = e.first;
				cloneElements[i].second= e.second->Clone();
				++i;
			}

			return cloneElements;
		}

		bool Middle() const override
		{
			return true;
		}

		vector<Key> Keys() const override
		{
			return MinSon()->Keys();
		}

		Key const& MinKey() const override
		{
			return _elements[0].first;
		}

		// TODO use macro to simplify below methods' content
		bool ContainsKey(Key const& key) const override
		{
			auto i = _elements.SuitPosition<true>(key);
			return _elements[i].second->ContainsKey(key);
		}

		Value GetValue(Key const& key) const override
		{
			auto i = _elements.SuitPosition<true>(key);
			return _elements[i].second->GetValue(key);
		}

		void ModifyValue(Key const& key, Value value) override
		{
			auto i = _elements.SuitPosition<true>(key);
			return _elements[i].second->ModifyValue(key, move(value)); // TODO can return void?
		}

		void Add(pair<Key, Value> p) override
		{
			auto i = _elements.SuitPosition<true>(p.first);
			_elements[i].second->Add(move(p));
		}

		void Remove(Key const& key) override
		{
			// auto k = key;// TODO how to convert reference type to reference_wrapper type
			auto i = _elements.SuitPosition<true>(key);
			_elements[i].second->Remove(key);
		}

	private:
		Base* MinSon() const
		{
			return _elements[0].second.get();
		}

		// TODO maybe add and remove are in the same method
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
			if (!node->Middle())
			{
				// Fresh the _next and _previous value
				// Set the two value in construct method, too
			}
			_elements.RemoveAt(_elements.IndexKeyOf(node->MinKey()));
			// Below two variables is to macro
			auto _next = _queryNext(this);
			auto _previous = _queryPrevious(this);
			REMOVE_COMMON;
		}

		// For sub node
		MiddleNode* QueryPrevious(MiddleNode* subNode)
		{
			auto i = _elements.IndexKeyOf(subNode->MinKey());
			if (i != 0) { return static_cast<MiddleNode *>(_elements[i - 1].second.get()); }
			return nullptr;
		}

		// For sub node
		MiddleNode* QueryNext(MiddleNode* subNode)
		{
			auto i = _elements.IndexKeyOf(subNode->MinKey());
			if (i != _elements.Count() - 1) { return static_cast<MiddleNode *>(_elements[i + 1].second.get()); }
			return nullptr;
		}

		void SetSubNodeCallback()
		{
			using ::std::placeholders::_1;
			using ::std::placeholders::_2;
			auto f1 = bind(&MiddleNode::AddSubNodeCallback, this, _1, _2);
			auto f2 = bind(&MiddleNode::DeleteSubNodeCallback, this, _1);
			for (auto& e : _elements)
			{
				auto& node = e.second;
				node->SetUpNodeCallback(f1, f2);
				if (node->Middle())
				{
					static_cast<MiddleNode *>(node.get())->_queryNext = 
						bind(&MiddleNode::QueryNext, this, _1);
					static_cast<MiddleNode *>(node.get())->_queryPrevious = 
						bind(&MiddleNode::QueryPrevious, this, _1);
				}
			}
		}

		static typename decltype(_elements)::Item ConvertToKeyBasePtrPair(unique_ptr<Base>& node)
		{
			using pairType = typename decltype(_elements)::Item;
			return make_pair<pairType::first_type, pairType::second_type>(cref(node->MinKey()), move(node));
		}
	};
}