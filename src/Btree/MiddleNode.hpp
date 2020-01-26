#pragma once
#include <memory>
#include <functional>
#include "Basic.hpp"
#include "EnumeratorPipeline.hpp"
#include "Elements.hpp"
#include "NodeBase.hpp"

namespace Collections 
{
	using ::std::move;
	using ::std::unique_ptr;
	using ::std::cref;
	using ::std::reference_wrapper;
	using ::std::pair;
	using ::std::make_pair;
	using ::std::bind;

	template <typename Key, typename Value, order_int BtreeOrder>
	class MiddleNode : public NodeBase<Key, Value, BtreeOrder>
	{
	private:
		using Base = NodeBase<Key, Value, BtreeOrder>;
		using _LessThan = LessThan<Key>;
		Elements<reference_wrapper<Key const>, unique_ptr<Base>, BtreeOrder, _LessThan> _elements;

		static typename decltype(_elements)::Item ConvertToKeyBasePtrPair(unique_ptr<Base> node)
		{
			using pairType = typename decltype(_elements)::Item;
			return make_pair<pairType::first_type, pairType::second_type>(cref(node->MinKey()), move(node));
		}

	public:
		MiddleNode(shared_ptr<_LessThan> lessThanPtr)
			: Base(), _elements(lessThanPtr)
		{
			SetSubNodeCallback();
		}	

		template <typename Iterator>
		MiddleNode(Enumerator<unique_ptr<Base>, Iterator> enumerator, shared_ptr<_LessThan> lessThanPtr)
			: Base(), _elements(EnumeratorPipeline<unique_ptr<Base>, typename decltype(_elements)::Item>(enumerator, [](unique_ptr<Base> node)
				{
					using pairType = typename decltype(_elements)::Item;
					return make_pair<pairType::first_type, pairType::second_type>(cref(node->MinKey()), move(node));
				}), lessThanPtr)
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
				cloneOne->_elements.Append(ConvertToKeyBasePtrPair(move(e.second->Clone())));
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

		bool ContainsKey(Key const&) const override
		{
			throw NotImplementException();
		}

		Value GetValue(Key const& key) const override
		{
			throw NotImplementException();
		}

		void ModifyValue(Key const& key, Value value) override
		{
			throw NotImplementException();
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
			// find index of srcNode and add new NextNode
			// if this is Full(), combine the node or call the upper node callback
		}

		void DeleteSubNodeCallback(Base* node)
		{
			// TODO
		}

		void SetSubNodeCallback()
		{
			//using ::std::placeholders::_1;
			//using ::std::placeholders::_2;
			//auto f1 = bind(&MiddleNode::AddSubNodeCallback, *this, _1, _2);
			//auto f2 = bind(&MiddleNode::DeleteSubNodeCallback, *this, _1);
			//for (auto& e : _elements)
			//{
			//	e.second->SetUpNodeCallback(f1, f2);
			//}
		}
	};
}