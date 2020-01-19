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
	
	/// PtrTo type should implement the method clone
	template <typename Key, typename Value, order_int BtreeOrder, typename PtrTo>
	class Elements
	{
	public:
		using VariantValue = variant<Value, unique_ptr<PtrTo>>;
		using Item = pair<Key, VariantValue>;
		using LessThan = function<bool(Key const&, Key const&)>;
		bool const           MiddleFlag;
		shared_ptr<LessThan> LessThanPtr;
	private:
		order_int               _count{ 0 };
		array<Item, BtreeOrder> _elements;

	public:
		template <typename Iter>
		Elements(Iter begin, Iter end, shared_ptr<LessThan> lessThanPtr)
			: MiddleFlag(!std::is_same_v<typename std::decay_t<decltype(*begin)>, pair<Key, Value>>),
			LessThanPtr(lessThanPtr)
		{
			do
			{
				_elements[_count++] = move(*begin);
				++begin;
			} while (begin != end);
		}

		template <typename T, typename Iterator>
		Elements(Enumerator<pair<Key, T>, Iterator> enumerator, shared_ptr<LessThan> lessThanPtr)
			: MiddleFlag(!std::is_same_v<typename std::decay_t<pair<Key, T>>, pair<Key, Value>>),
			LessThanPtr(lessThanPtr)
		{
			while (enumerator.MoveNext())
			{
				_elements[_count++] = move(enumerator.Current());
			}
		}

		Elements(Elements const& that)
			: MiddleFlag(that.MiddleFlag), LessThanPtr(that.LessThanPtr),
			_elements(move(that.CloneInternalElements())), _count(that._count)
		{}

		Elements(Elements&& that) noexcept
			: MiddleFlag(that.MiddleFlag),
			LessThanPtr(that.LessThanPtr),
			_elements(move(that._elements)),
			_count(that._count)
		{
			that._count = 0;
		}

		bool Have(Key const& key) const
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

		/// Remove the item corresponding to the key.
		/// Invoker should ensure key exists in this Elements.
		/// \param key
		/// \return bool means max item changes or not
		bool Remove(Key const& key)
		{
			return RemoveAt(IndexOf(key));
		}

		/// Remove the item corresponding to the index.
		/// Invoker should ensure i is within range.
		/// \param i index
		/// \return bool means max item changes or not
		bool RemoveAt(order_int i)
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

		void Insert(pair<Key, Value>);
		void Insert(pair<Key, unique_ptr<PtrTo>>);
		void Append(pair<Key, Value>);
		void Append(pair<Key, unique_ptr<PtrTo>>);

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

		pair<Key, Value> ExchangeMax(pair<Key, Value>);
		pair<Key, unique_ptr<PtrTo>> ExchangeMax(pair<Key, unique_ptr<PtrTo>>);
		pair<Key, Value> ExchangeMin(pair<Key, Value>, bool& maxChanged);
		pair<Key, unique_ptr<PtrTo>> ExchangeMin(pair<Key, unique_ptr<PtrTo>>, bool& maxChanged);

		VariantValue const& operator[](Key const& key) const
		{
			return this->operator[](IndexOf(key)).second;
		}

		VariantValue& operator[](Key const& key)
		{
			return const_cast<VariantValue&>(
				(static_cast<const Elements&>(*this))[key]
				);
		}

		const Item& operator[](order_int i) const
		{
			return _elements[i];
		}

		Item& operator[](order_int i)
		{
			return const_cast<Item&>(
				(static_cast<const Elements&>(*this))[i]
				);
		}

		order_int IndexOf(PtrTo* pointer) const
		{
			auto enumerator = GetEnumerator();
			while (enumerator.MoveNext())
			{
				auto& item = enumerator.Current();
				if (ptr(item.second) == pointer)
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

		template <typename T>
		pair<Key, T> ExchangeMax(pair<Key, T> p)
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

		template <typename T>
		pair<Key, T> ExchangeMin(pair<Key, T> p, bool& maxChanged)
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

		template <typename T>
		void Append(pair<Key, T> p)
		{
			_elements[_count++] = move(p);
		}

		template <typename T>
		void Insert(pair<Key, T> p)
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

		template <typename T>
		bool Add(pair<Key, T> p)
		{
			if ((*LessThanPtr)(_elements[_count - 1].first, p.first))
			{
				Append(move(p));
				return true;
			}
			else
			{
				Insert(move(p));
				return false;
			}
		}

		auto CloneInternalElements() const
		{
			decltype(_elements) copy;
			for (auto i = 0; i < _count; ++i)
			{
				copy[i].first = _elements[i].first;
				if (MiddleFlag)
				{
					copy[i].second = move(uniquePtr_Ref(_elements[i].second)->Clone());
				}
				else
				{
					copy[i].second = value_Ref(_elements[i].second);
				}
			}

			return move(copy);
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
			if (direction < 0)
			{
				auto e = end();
				for (auto begin = start; begin != e; ++begin)
				{
					*(begin + direction) = move(*begin);
				}
			}
			else if (direction > 0)
			{
				decltype(_elements.rend()) rend{ start - 1 };
				for (auto rbegin = _elements.rbegin(); rbegin != rend; ++rbegin)
				{
					*(rbegin + direction) = move(*rbegin);
				}
			}
		}

		static unique_ptr<PtrTo>& uniquePtr_Ref(VariantValue& v)
		{
			return std::get<unique_ptr<PtrTo>>(v);
		}

		static const unique_ptr<PtrTo>& uniquePtr_Ref(const VariantValue& v)
		{
			return std::get<unique_ptr<PtrTo>>(v);
		}

		static unique_ptr<PtrTo> uniquePtr_Move(VariantValue& v)
		{
			return move(uniquePtr_Ref(v));
		}

	public:
		/// Get Value reference in VariantValue
		/// \param v
		/// \return Value reference
		static Value& value_Ref(VariantValue& v)
		{
			return std::get<Value>(v);
		}

		/// Const version of value_Ref
		/// \param v
		/// \return
		static Value const& value_Ref(const VariantValue& v)
		{
			return std::get<Value>(v);
		}

		/// Get Value copy in VariantValue
		/// \param v
		/// \return Value copy
		static Value value_Copy(const VariantValue& v)
		{
			return std::get<Value>(v);
		}

		/// Move the Value out in VariantValue
		/// \param v
		/// \return Value moved from VariantValue
		static Value value_Move(VariantValue& v)
		{
			return move(std::get<Value>(v));
		}

		/// Get the pointer in VariantValue
		/// \param v
		/// \return Pointer
		static PtrTo* ptr(VariantValue const& v)
		{
			return std::get<unique_ptr<PtrTo>>(v).get();
		}
	};
}

namespace Collections
{
	using ::std::move;

#define ELEMENTS_TEMPLATE template <typename Key, typename Value, order_int BtreeOrder, typename PtrTo>
#define ELE Elements<Key, Value, BtreeOrder, PtrTo>

#define VOID_RET_MODIFY_METHOD_INSTANCE(METHOD, T) ELEMENTS_TEMPLATE void ELE::METHOD(pair<Key, T> p) { return METHOD<T>(move(p)); }

		VOID_RET_MODIFY_METHOD_INSTANCE(Insert, Value)
		VOID_RET_MODIFY_METHOD_INSTANCE(Insert, unique_ptr<PtrTo>)

		VOID_RET_MODIFY_METHOD_INSTANCE(Append, Value)
		VOID_RET_MODIFY_METHOD_INSTANCE(Append, unique_ptr<PtrTo>)

#undef VOID_RET_MODIFY_METHOD_INSTANCE

#define EXCHANGE_MAX_INSTANCE(T) ELEMENTS_TEMPLATE pair<Key, T> ELE::ExchangeMax(pair<Key, T> p) { return ExchangeMax<T>(move(p)); }

		EXCHANGE_MAX_INSTANCE(Value)
		EXCHANGE_MAX_INSTANCE(unique_ptr<PtrTo>)

#undef EXCHANGE_MAX_INSTANCE

#define EXCHANGE_MIN_INSTANCE(T) ELEMENTS_TEMPLATE pair<Key, T> ELE::ExchangeMin(pair<Key, T> p, bool &maxChanged) { return ExchangeMin<T>(move(p), maxChanged); }


		EXCHANGE_MIN_INSTANCE(Value)
		EXCHANGE_MIN_INSTANCE(unique_ptr<PtrTo>)

#undef EXCHANGE_MIN_INSTANCE
#undef ELE
#undef ELEMENTS_TEMPLATE
}