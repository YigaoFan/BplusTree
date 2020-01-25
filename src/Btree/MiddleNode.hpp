#pragma once
#include <memory>
#include <functional>
#include "Basic.hpp"
#include "Elements.hpp"
#include "NodeBaseCrtp.hpp"

namespace Collections 
{
	using ::std::move;
	using ::std::unique_ptr;
	using ::std::cref;
	using ::std::reference_wrapper;
	using ::std::pair;
	using ::std::make_pair;

#define MIDDLE MiddleNode<Key, Value, BtreeOrder>
	template <typename Key, typename Value, order_int BtreeOrder>
	class MiddleNode : public NodeBase_CRTP<MIDDLE, Key, Value, BtreeOrder>
#undef MIDDLE
	{
	private:
		using          Base = NodeBase<Key, Value, BtreeOrder>;
		using          Base_CRTP = NodeBase_CRTP<MiddleNode, Key, Value, BtreeOrder>;
		using _LessThan = LessThan<Key>;
		Elements<reference_wrapper<Key>, unique_ptr<Base>, BtreeOrder> _elements;

		static pair<reference_wrapper<Key>, unique_ptr<Base>> ConvertToKeyBasePtrPair(unique_ptr<Base> node)
		{
			return make_pair<reference_wrapper<Key>, unique_ptr<Base>>(cref(node->MinKey()), move(node));
		}

	public:
		template <typename Iterator>
		MiddleNode(Enumerator<unique_ptr<Base>, Iterator> enumerator, shared_ptr<_LessThan> lessThanPtr)
			: Base_CRTP(), _elements(EnumeratorPipeine(enumerator, ConvertToKeyBasePtrPair), lessThanPtr)
		{
			for (auto& e : _elements)
			{
				e.second.SetUpNodeCallback(AddSubNodeCallback, DeleteSubNodeCallback);
			}
		}

		MiddleNode(MiddleNode const& that)
			: Base_CRTP(that)// TODO how to solve _elements copy
		{ }

		MiddleNode(MiddleNode&& that) noexcept
			: Base_CRTP(move(that)), _elements(move(that._elements))
		{ }

		~MiddleNode() override = default;

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
			auto k = key;// TODO how to convert reference type to reference_wrapper type
			auto i = _elements.SuitPosition<true>(k/*ey*/);
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