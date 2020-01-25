#pragma once
#include <functional>
#include <vector>
#include <memory>
#include <utility>
#include "Basic.hpp"
#include "IEnumerator.hpp"
#include "Exception.hpp"

namespace Collections
{
	using ::std::pair;
	using ::std::vector;
	using ::std::function;
	using ::std::shared_ptr;
	using ::std::array;
	using ::std::move;

	enum FindResult
	{
		EqualFound,
		MaybeExistInDeep,
		NotExist,
	};
	//struct TailAppendWay {};
	//struct HeadInsertWay {};
	
	// TODO when BtreeOrder is big, use binary search in iterate process
	template <typename Key, typename Value, order_int BtreeOrder>
	class Elements
	{
	public:
		using Item = pair<Key, Value>;
		using LessThan = function<bool(Key const&, Key const&)>;
		shared_ptr<LessThan> LessThanPtr;
	private:
		order_int               _count{ 0 };
		array<Item, BtreeOrder> _elements;

	public:
		Elements(shared_ptr<LessThan> lessThanPtr)
			: LessThanPtr(lessThanPtr)
		{ }

		Elements(IEnumerator<pair<Key, Value>>& enumerator, shared_ptr<LessThan> lessThanPtr)
			: LessThanPtr(lessThanPtr)
		{
			while (enumerator.MoveNext())
			{
				_elements[_count++] = move(enumerator.Current());
			}
		}

		// TODO here maybe should care about Middle Node copy
		Elements(Elements const& that)
			: LessThanPtr(that.LessThanPtr), _elements(that._elements), _count(that._count)
		{}

		Elements(Elements&& that) noexcept
			:  LessThanPtr(that.LessThanPtr), _elements(move(that._elements)), _count(that._count)
		{
			that._count = 0;
		}

		bool ContainsKey(Key const& key) const
		{
			auto enumerator = GetEnumerator();
			while (enumerator.MoveNext())
			{
				auto& e = enumerator.Current();
				auto& lessThan = *LessThanPtr;
				if (auto less = lessThan(key, e.first), notLess = lessThan(e.first, key); less == notLess)
				{
					return true;
				}
				else if (notLess)
				{
					break;
				}
			}

			return false;
		}

		order_int Count() const
		{
			return _count;
		}

		bool Full() const
		{
			return _count == BtreeOrder;
		}
		
		vector<Key> Keys() const
		{
			vector<Key> keys;
			keys.reserve(_count);
			auto enumerator = GetEnumerator();
			while (enumerator.MoveNext())
			{
				keys.push_back(enumerator.Current().first);
			}

			return keys;
		}

		/// Remove the item corresponding to the key.
		/// Invoker should ensure key exists in this Elements.
		/// \param key
		void Remove(Key const& key)
		{
			return RemoveAt(IndexOf(key));
		}

		/// Remove the item corresponding to the index.
		/// Invoker should ensure i is within range.
		/// \param i index
		void RemoveAt(order_int i)
		{
			MoveItems(-1, i + 1); // TODO this remove should think of the destruct problem
			--_count;
			//return i == (_count - 1);
		}

		// TODO check ref exist?
		template <bool FromHead>
		void RemoveItems(order_int count)
		{
			_count -= count;
			if constexpr (FromHead)
			{
				MoveItems(-count, count);
			}
			else
			{
				for (auto rbegin = _elements.rbegin(); count != 0; --count, --rbegin)
				{
					rbegin->~Item();
				}
			}
		}

		vector<pair<Key, Value>> PopOutItems(order_int count)
		{
			vector<pair<Key, Value>> outItems;
			while (count != 0)
			{
				outItems.emplace_back(move(_elements[_count - 1]));
				--_count;
				--count;
			}

			return outItems;
		}

		pair<Key, Value> FrontPopOut()
		{
			auto p = move(_elements[0]);
			RemoveAt(0);
			return p;
		}

		void Add(pair<Key, Value> p)
		{
			if ((*LessThanPtr)(_elements[_count - 1].first, p.first))
			{
				Append(move(p));
			}
			else
			{
				Insert(move(p));
			}
		}

		void Add(vector<pair<Key, Value>> pairs)
		{
			for (auto& p : pairs)
			{
				Add(move(p));
			}
		}

		void Insert(pair<Key, Value> p)
		{
			for (order_int i = 0; i < _count; ++i)
			{
				if ((*LessThanPtr)(p.first, _elements[i].first))
				{
					MoveItems(1, i);
					_elements[i] = move(p);
					++_count;
				}
			}
		}

		void Append(pair<Key, Value> p)
		{
			_elements[_count++] = move(p);
		}

		//order_int ChangeKeyOf(Value const& value, Key newKey)
		//{
		//	auto index = IndexOf(value);
		//	_elements[index].first = move(newKey);
		//	return index;
		//}

		pair<Key, Value> ExchangeMax(pair<Key, Value> p)
		{
			auto maxItem = move(_elements[_count - 1]);
			--_count;
			Add(move(p));
			return maxItem;
		}

		pair<Key, Value> ExchangeMin(pair<Key, Value> p)
		{
			auto minItem = move(_elements[0]);
			MoveItems(-1, 1);
			--_count;
			Add(move(p));
			return minItem;
		}

		Value const& operator[](Key const& key) const
		{
			return this->operator[](IndexOf(key)).second;
		}

		Value& operator[](Key const& key)
		{
			return const_cast<Value&>(
				(static_cast<const Elements&>(*this))[key]
				);
		}

		Item const& operator[](order_int i) const
		{
			return _elements[i];
		}

		Item& operator[](order_int i)
		{
			return const_cast<Item&>(
				(static_cast<const Elements&>(*this))[i]
				);
		}

		// TODO sometimes Key and Value are the same type, how to avoid
		//order_int IndexOf(Value const& value) const
		//{
		//	auto enumerator = GetEnumerator();
		//	while (enumerator.MoveNext())
		//	{
		//		auto& item = enumerator.Current();
		//		// TODO Value use this to compare maybe not right, need to care this method use in scene
		//		if (item.second == value)
		//		{
		//			return (order_int)enumerator.CurrentIndex();
		//		}
		//	}

		//	throw KeyNotFoundException();
		//}

		order_int IndexOf(Key const& key) const
		{
			auto enumerator = GetEnumerator();
			auto& lessThan = *LessThanPtr;
			while (enumerator.MoveNext())
			{
				auto& item = enumerator.Current();
				if (lessThan(key, item.first) == lessThan(item.first, key))
				{
					return (order_int)enumerator.CurrentIndex();
				}
			}

			throw KeyNotFoundException();
		}

		/// Only suit for ptr stored in.
		//FindResult TryIndexOf(Key const& key, order_int& resultIndex) const
		//{
		//	auto& lessThan = *LessThanPtr;
		//	for (decltype(_count) i = 1; i < _count; ++i)
		//	{
		//		auto& e = _elements[i];
		//		if (lessThan(key, e.first))
		//		{
		//			resultIndex = i - 1;
		//			return FindResult::MaybeExistInDeep;
		//		}
		//		else if (!lessThan(e.first, key))
		//		{
		//			resultIndex = i - 1;
		//			return FindResult::EqualFound;
		//		}
		//	}

		//	return FindResult::NotExist;
		//}

		template <bool ChooseBranch>
		order_int SuitPosition(Key const& key) const
		{
			if constexpr (ChooseBranch)
			{
				for (decltype(_count) i = 1; i < _count; ++i)
				{
					if ((*LessThanPtr)(key, _elements[i].first))
					{
						return i - 1;
					}
				}

				return _count - 1;
			}
			else
			{
				for (decltype(_count) i = 0; i < _count; ++i)
				{
					if ((*LessThanPtr)(key, _elements[i].first))
					{
						return i;
					}
				}

				return _count;
			}

		}

		auto begin()
		{
			return _elements.begin();
		}

		auto end()
		{
			return begin() + _count;
		}

		auto begin() const
		{
			return _elements.begin();
		}

		auto end() const
		{
			return begin() + _count;
		}

	private:
		auto GetEnumerator()
		{
			return CreateEnumerator(_elements);
		}

		auto GetEnumerator() const
		{
			return CreateEnumerator(_elements);
		}

		void MoveItems(int32_t direction, order_int index)
		{
			MoveItems(direction, begin() + index);
		}

		/// Start included, still exist
		/// \param direction
		/// \param start
		void MoveItems(int32_t direction, decltype(_elements.begin()) start)
		{
			if (direction == 0) { return; }
			if (direction < 0)
			{
				auto e = end();
				for (auto begin = start; begin != e; ++begin)
				{
					*(begin + direction) = move(*begin);
				}
			}
			else
			{
				decltype(_elements.rend()) rend{ start - 1 };
				for (auto rbegin = _elements.rbegin(); rbegin != rend; ++rbegin)
				{
					*(rbegin + direction) = move(*rbegin);
				}
			}
		}
	};
}