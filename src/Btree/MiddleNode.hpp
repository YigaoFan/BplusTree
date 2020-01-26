#pragma once
#include <memory>
#include <functional>
#include "Basic.hpp"
#include "Elements.hpp"
#include "NodeBase.hpp"
// #include "NodeBaseCrtp.hpp"

namespace Collections 
{
	using ::std::move;
	using ::std::unique_ptr;
	using ::std::cref;
	using ::std::reference_wrapper;
	using ::std::pair;
	using ::std::make_pair;

	template <typename Key, typename Value, order_int BtreeOrder>
	class MiddleNode : public NodeBase<Key, Value, BtreeOrder>
	{
	private:
		using Base = NodeBase<Key, Value, BtreeOrder>;
		using _LessThan = LessThan<Key>;
		Elements<reference_wrapper<Key>, unique_ptr<Base>, BtreeOrder> _elements;

		static pair<decltype(_elements)::Item> ConvertToKeyBasePtrPair(unique_ptr<Base> node)
		{
			return make_pair<decltype(_elements)::Item>(cref(node->MinKey()), move(node));
		}

	public:
		MiddleNode(shared_ptr<_LessThan> lessThanPtr)
			: Base(), _elements(lessThanPtr)
		{
			for (auto& e : _elements)
			{
				e.second.SetUpNodeCallback(AddSubNodeCallback, DeleteSubNodeCallback);
			}
		}	

		template <typename Iterator>
		MiddleNode(Enumerator<unique_ptr<Base>, Iterator> enumerator, shared_ptr<_LessThan> lessThanPtr)
			: Base(), _elements(EnumeratorPipeine(enumerator, ConvertToKeyBasePtrPair), lessThanPtr)
		{
			for (auto& e : _elements)
			{
				e.second.SetUpNodeCallback(AddSubNodeCallback, DeleteSubNodeCallback);
			}
		}

		MiddleNode(MiddleNode const& that)
			: Base(that)// TODO how to solve _elements copy
		{ }

		MiddleNode(MiddleNode&& that) noexcept
			: Base(move(that)), _elements(move(that._elements))
		{ }

		~MiddleNode() override = default;

		unique_ptr<Base> Clone() const override
		{
			auto cloneOne = make_unique<MiddleNode>(_elements.LessThanPtr);
			for (auto const& e : _elements)
			{
				cloneOne->_elements.Append(ConvertToKeyBasePtrPair(move(e.second->Clone())));
			}

			return cloneOne;
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
	};
}