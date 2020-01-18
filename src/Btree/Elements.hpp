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

	struct TailAppendWay {};
	struct HeadInsertWay {};
	/**
	 * First and second relationship is to public
	 */
	template <typename Key, typename Value, order_int BtreeOrder, typename Ptr>
	class Elements
	{
	public:
		using VariantValue = variant<Value, unique_ptr<Ptr>>;
		using Item = pair<Key, VariantValue>;
		using LessThan = function<bool(Key const&, Key const&)>;
		const bool           MiddleFlag;
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
				_elements[_count] = move(*begin);
				++_count;
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
		{ }

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
			for (auto const& e : _elements)
			{
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
			return Remove(IndexOf(key));
		}

		/// Remove the item corresponding to the key.
		/// Invoker should ensure i is within range.
		/// \param i index
		/// \return bool means max item changes or not
		bool Remove(order_int i)
		{
			auto maxChanged = false;
			AdjustMemory(-1, i + 1);

			if (i == (_count - 1))
			{
				maxChanged = true;
			}
			--_count;

			return maxChanged;
		}

		template <bool FROM_HEAD>
		void RemoveItems(order_int count)
		{
			if constexpr (FROM_HEAD) {
				AdjustMemory(-count, count);
			}
			else
			{
				auto Num = count;
				for (auto rbegin = _elements.rbegin(); Num != 0; --Num, --rbegin)
				{
					rbegin->~Item();
				}
			}

			_count -= count;
		}

		void Insert(pair<Key, Value>);
		void Insert(pair<Key, unique_ptr<Ptr>>);
		void Append(pair<Key, Value>);
		void Append(pair<Key, unique_ptr<Ptr>>);

		void Receive(TailAppendWay, Elements&& that)
		{
			Receive(TailAppendWay(), that.Count(), that);
		}

		void Receive(HeadInsertWay, Elements&& that)
		{
			Receive(HeadInsertWay(), that.Count(), that);
		}

		void Receive(HeadInsertWay, order_int count, Elements& that)
		{
			AdjustMemory(count, count); // TODO check this work right
			decltype(that._elements.begin()) start = (that._elements.end() - count);
			auto end = start + count;

			for (auto i = 0; start != end; ++start, ++i)
			{
				_elements[i] = move(*start);
				++_count;
			}

			that.RemoveItems<false>(count); // will decrease preThat _count
		}

		// start "that" where is not clear
		void Receive(TailAppendWay, order_int count, Elements& that)
		{
			for (auto i = 0; i < count; ++i)
			{
				_elements[_count] = move(that._elements[i]);
				++_count;
			}

			that.RemoveItems<true>(count);
		}

		order_int ChangeKeyOf(Ptr *ptr, Key newKey)
		{
			auto index = IndexOf(ptr);
			_elements[index].first = move(newKey);

			return index;
		}

		pair<Key, Value> ExchangeMax(pair<Key, Value>);
		pair<Key, unique_ptr<Ptr>> ExchangeMax(pair<Key, unique_ptr<Ptr>>);
		pair<Key, Value> ExchangeMin(pair<Key, Value>, bool &maxChanged);
		pair<Key, unique_ptr<Ptr>> ExchangeMin(pair<Key, unique_ptr<Ptr>>, bool &maxChanged);

		VariantValue& operator[](Key const& key)
		{
			return const_cast<VariantValue&>(
				(static_cast<const Elements&>(*this))[key]
				);
		}

		Item& operator[](order_int i)
		{
			return const_cast<Item&>(
				(static_cast<const Elements&>(*this))[i]
				);
		}

		VariantValue const& operator[](Key const& key) const
		{
			auto i = IndexOf(key);
			if (i != -1)
			{
				return _elements[i].second;
			}

			throw runtime_error("Can't get the Value correspond"
				+ key
				+ ","
				+ " Please check the key existence.");
		}

		const Item& operator[](order_int i) const
		{
			return _elements[i];
		}

		order_int IndexOf(Ptr* pointer) const
		{
			for (decltype(_count) i = 0; i < _count; ++i)
			{
				if (ptr(_elements[i].second) == pointer)
				{
					return i;
				}
			}

			throw KeyNotFoundException();
		}

		order_int IndexOf(Key const& key) const
		{
			auto& lessThan = *LessThanPtr;
			for (decltype(_count) i = 0; i < _count; ++i)
			{
				if (lessThan(key, _elements[i].first) == lessThan(_elements[i].first, key))
				{
					return i;
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
		static Value const& value_Ref(const VariantValue &v)
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
		static Ptr* ptr(VariantValue const& v)
		{
			return std::get<unique_ptr<Ptr>>(v).get();
		}

	private:
		template <typename T>
		pair<Key, T> ExchangeMax(pair<Key, T> p)
		{
			auto& maxItem = _elements[_count - 1];
			auto key = maxItem.first;
			auto valueForContent = move(maxItem.second);

			--_count;
			Add(move(p));

			if constexpr (std::is_same<T, Value>::value)
			{
				return make_pair<Key, T>(move(key), value_Move(valueForContent));
			}
			else
			{
				return make_pair<Key, T>(move(key), uniquePtr_Move(valueForContent));
			}
		}

		template <typename T>
		pair<Key, T> ExchangeMin(pair<Key, T> p, bool &maxChanged)
		{
			auto& minItem = _elements[0];
			auto key = move(minItem.first);
			auto valueForContent = move(minItem.second);

			// move left
			AdjustMemory(-1, 1);
			--_count;

			maxChanged = Add(move(p));

			if constexpr (std::is_same<T, Value>::value) {
				return make_pair<Key, T>(move(key), move(value_Ref(valueForContent)));
			}
			else
			{
				return make_pair<Key, T>(move(key), uniquePtr_Move(valueForContent));
			}
		}

		template <typename T>
		void Append(pair<Key, T> p)
		{
			if (Full())
			{
				throw runtime_error("No free space to add");
			}

			_elements[_count] = move(p);
			++_count;
		}

		template <typename T>
		void Insert(pair<Key, T> p)
		{
			// maybe only leaf add logic use this Elements method
			// middle add logic is in middle Node self
			auto& k = p.first;
			auto& v = p.second;

			uint16_t i = 0;
			for (; i < _count; ++i)
			{
				if ((*LessThanPtr)(k, _elements[i].first) == (*LessThanPtr)(_elements[i].first, k))
				{
					throw runtime_error("The inserting key duplicates");
				}
				else if ((*LessThanPtr)(k, _elements[i].first))
				{
					goto Insert;
				}
			}
			// when equal and bigger than the bound, throw the error
			throw runtime_error(/*"\nWrong adding Key-Value: " + k + " " + v + ". "
							+ "Now the count of this Elements: " + std::to_string(_count)+ ". "
							+ */"And please check the max Key to ensure not beyond the bound.");

			Insert:
			AdjustMemory(1, i);
			_elements[i] = move(p);
			++_count;
		}

		template <typename T>
		bool Add(pair<Key, T> p)
		{
			auto& lessThan = *LessThanPtr;

			if (lessThan(_elements[_count - 1].first, p.first))
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

		void AdjustMemory(int32_t direction, uint16_t index)
		{
			MoveElement(direction, begin() + index);
		}

		auto CloneInternalElements() const
		{
			decltype(_elements) copy;
			for (auto i = 0; i < _count; ++i)
			{
				copy[i].first = _elements[i].first;
				if (MiddleFlag)
				{
					copy[i].second = move(uniquePtr_Ref(_elements[i].second)->clone());
				}
				else
				{
					copy[i].second = value_Ref(_elements[i].second);
				}
			}

			return move(copy);
		}

		/// Start included, still exist
		/// \param direction
		/// \param start
		void MoveElement(int32_t direction, decltype(_elements.begin()) start)
		{
			if (direction < 0)
			{
				auto end = this->end();

				for (auto& begin = start; begin != end; ++begin)
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

		static unique_ptr<Ptr>& uniquePtr_Ref(VariantValue& v)
		{
			return std::get<unique_ptr<Ptr>>(v);
		}

		static const unique_ptr<Ptr>& uniquePtr_Ref(const VariantValue& v)
		{
			return std::get<unique_ptr<Ptr>>(v);
		}

		static unique_ptr<Ptr> uniquePtr_Move(VariantValue& v)
		{
			return move(uniquePtr_Ref(v));
		}
	};
}

namespace Collections
{
#define ELEMENTS_TEMPLATE template <typename Key, typename Value, order_int BtreeOrder, typename Ptr>
#define ELE Elements<Key, Value, BtreeOrder, Ptr>

#define VOID_RET_MODIFY_METHOD_INSTANCE(METHOD, T) ELEMENTS_TEMPLATE void ELE::METHOD(pair<Key, T> p) { return METHOD<T>(move(p)); }

	VOID_RET_MODIFY_METHOD_INSTANCE(Insert, Value)
	VOID_RET_MODIFY_METHOD_INSTANCE(Insert, unique_ptr<Ptr>)

	VOID_RET_MODIFY_METHOD_INSTANCE(Append, Value)
	VOID_RET_MODIFY_METHOD_INSTANCE(Append, unique_ptr<Ptr>)

#undef VOID_RET_MODIFY_METHOD_INSTANCE

#define EXCHANGE_MAX_INSTANCE(T) ELEMENTS_TEMPLATE pair<Key, T> ELE::ExchangeMax(pair<Key, T> p) { return ExchangeMax<T>(move(p)); }

	EXCHANGE_MAX_INSTANCE(Value)
	EXCHANGE_MAX_INSTANCE(unique_ptr<Ptr>)

#undef EXCHANGE_MAX_INSTANCE

#define EXCHANGE_MIN_INSTANCE(T) ELEMENTS_TEMPLATE pair<Key, T> ELE::ExchangeMin(pair<Key, T> p, bool &maxChanged) { return ExchangeMin<T>(move(p), maxChanged); }

	EXCHANGE_MIN_INSTANCE(Value)
	EXCHANGE_MIN_INSTANCE(unique_ptr<Ptr>)

#undef EXCHANGE_MIN_INSTANCE
}
#undef ELE
#undef ELEMENTS_TEMPLATE