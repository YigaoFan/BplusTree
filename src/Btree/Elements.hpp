#pragma once
#include <functional>
#include <variant>
#include <exception>
#include <string> // For to_string
#include <vector>
#include <memory>
#include <iterator>
#include <utility>
#include "Basic.hpp"
#include "Exception.hpp"

namespace Collections
{
	using ::std::variant;
	using ::std::pair;
	using ::std::vector;
	using ::std::function;
	using ::std::unique_ptr;
	using ::std::shared_ptr;
	using ::std::array;
	using ::std::runtime_error;
	using ::std::make_unique;
	using ::std::forward_iterator_tag;
	using ::std::make_pair;
	using ::std::move;
	using ::std::is_same_v;

	struct TailAppendWay {};
	struct HeadInsertWay {};
	
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
		template <typename Iter>
		Elements(Iter begin, Iter end, shared_ptr<LessThan> lessThanPtr)
			: LessThanPtr(lessThanPtr)
		{
			do
			{
				_elements[_count++] = move(*begin);
				++begin;
			} while (begin != end);
		}

		template <typename Value, typename Iterator>
		Elements(Enumerator<pair<Key, Value>, Iterator> enumerator, shared_ptr<LessThan> lessThanPtr)
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
			MoveItems(-1, i + 1);
			--_count;
			return i == (_count - 1);
		}

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

		template <bool AtHead>
		void Receive(Elements&& that)
		{
			if constexpr (AtHead)
			{
				Receive(HeadInsertWay(), that.Count(), that);
			}
			else
			{
				Receive(TailAppendWay(), that.Count(), that);
			}
		}

		void Receive(HeadInsertWay, order_int count, Elements& that)
		{
			MoveItems(count, 0);
			auto end = that._elements.end();
			auto start = (end - count);

			for (auto i = 0; start != end; ++start, ++i)
			{
				_elements[i] = move(*start);
			}

			_count += count;
			that.RemoveItems<false>(count); // Will decrease preThat _count
		}

		void Receive(TailAppendWay, order_int count, Elements& that)
		{
			for (auto i = 0; i < count; ++i)
			{
				_elements[_count++] = move(that._elements[i]);
			}

			that.RemoveItems<true>(count);
		}

		order_int ChangeKeyOf(PtrTo* ptr, Key newKey)
		{
			auto index = IndexOf(ptr);
			_elements[index].first = move(newKey);
			return index;
		}

		pair<Key, Value> ExchangeMax(pair<Key, Value> p)
		{
			auto& maxItem = _elements[_count - 1];
			auto key = move(maxItem.first);
			auto variantValue = move(maxItem.second);
			--_count;
			Add(move(p));

			if constexpr (is_same_v<decay_t<T>, Value>)
			{
				return make_pair<Key, T>(move(key), value_Move(variantValue));
			}
			else
			{
				return make_pair<Key, T>(move(key), uniquePtr_Move(variantValue));
			}
		}

		pair<Key, Value> ExchangeMin(pair<Key, Value> p)
		{
			auto& minItem = _elements[0];
			auto key = move(minItem.first);
			auto variantValue = move(minItem.second);
			MoveItems(-1, 1);
			--_count;
			maxChanged = Add(move(p));

			if constexpr (is_same_v<decay_t<T>, Value>)
			{
				return make_pair<Key, T>(move(key), value_Move(variantValue));
			}
			else
			{
				return make_pair<Key, T>(move(key), uniquePtr_Move(variantValue));
			}
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

		order_int IndexOf(Value const& value) const
		{
			auto enumerator = GetEnumerator();
			while (enumerator.MoveNext())
			{
				auto& item = enumerator.Current();
				// TODO Value use this to compare maybe not right, need to care this method use in scene
				if (item.second == value)
				{
					return (order_int)enumerator.CurrentIndexFromStart();
				}
			}

			throw KeyNotFoundException();
		}

		order_int IndexOf(Key const& key) const
		{
			auto enumerator = GetEnumerator();
			auto& lessThan = *LessThanPtr;
			while (enumerator.MoveNext())
			{
				auto& item = enumerator.Current();
				if (lessThan(key, item.first) == lessThan(item.first, key))
				{
					return (order_int)enumerator.CurrentIndexFromStart();
				}
			}

			throw KeyNotFoundException();
		}

		order_int SuitPosition(Key const& key) const
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