#pragma once
#include <functional> // for function
#include <variant>    // variant
#include <cstring>    // for memcpy
#include <exception>  // for runtime_error
#include <string>     // for to_string
#include <vector>     // for vector
#include <memory>     // for unique_ptr, shared_ptr
#include <iterator>
#include <utility>
#ifdef BTREE_DEBUG
#include "Utility.hpp"
#endif

namespace Collections {
	using ::std::variant;
	using ::std::pair;
	using ::std::vector;
	using ::std::function;
	using ::std::unique_ptr;
	using ::std::shared_ptr;
	using ::std::array;
	using ::std::memcpy;
	using ::std::runtime_error;
	using ::std::make_unique;
	using ::std::forward_iterator_tag;
	using ::std::make_pair;

#define ELEMENTS_TEMPLATE template <typename Key, typename Value, uint16_t BtreeOrder, typename PtrType>

	struct TailAppendWay {};
	struct HeadInsertWay {};
	// Internal could use ptr to search when know ptr
	// modify method arg should be pair<Key, Value> without reference
	/**
	 * First and second relationship is to public
	 */
	ELEMENTS_TEMPLATE
		class Elements
	{
	public:
		using ValueForContent = variant<Value, unique_ptr<PtrType>>;
		using Content = pair<Key, ValueForContent>;
		using LessThan = function<bool(const Key&, const Key&)>;

		const bool           MiddleFlag;
		shared_ptr<LessThan> LessThanPtr;

		template <typename Iter>
		Elements(Iter begin, Iter end, shared_ptr<LessThan> lessThanPtr)
			:
			MiddleFlag(!std::is_same<typename std::decay_t<decltype(*begin)>, pair<Key, Value>>::value),
			LessThanPtr(lessThanPtr),
			_count(0)
		{
			do
			{
				_elements[_count] = std::move(*begin);

				++_count;
				++begin;
			} while (begin != end);
		}

		Elements(const Elements& that)
			: MiddleFlag(that.MiddleFlag), LessThanPtr(that.LessThanPtr),
			_elements(std::move(that.cloneInternalElements())), _count(that._count)
		{ }

		Elements(Elements&& that) noexcept :
			MiddleFlag(that.MiddleFlag),
			LessThanPtr(that.LessThanPtr),
			_elements(std::move(that._elements)),
			_count(that._count)
		{
			that._count = 0;
		}

		bool have(const Key& key) const
		{
			auto& lessThan = *LessThanPtr;

			for (const auto& e : _elements)
			{
				auto& k = e.first;
				auto less = lessThan(key, k);
				auto notLess = lessThan(k, key);

				if (less == notLess)
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

		uint16_t count() const
		{
			return _count;
		}

		bool full() const
		{
			return _count == BtreeOrder;
		}

		// bool means max key changes
		// all this change caller should promise not out of bound
		bool remove(const Key& key)
		{
			auto i = indexOf(key);

			if (i != -1)
			{
				return remove(i);
			}

			return false;
		}

		bool remove(uint16_t i)
		{
			auto maxChanged = false;

			adjustMemory(-1, i + 1);

			if (i == (_count - 1))
			{
				maxChanged = true;
			}
			--_count;

			return maxChanged;
		}

		template <bool FROM_HEAD>
		void removeItems(uint16_t count)
		{
			if constexpr (FROM_HEAD) {
				adjustMemory(-count, count);
			}
			else
			{
				auto Num = count;
				for (auto rbegin = _elements.rbegin(); Num != 0; --Num, --rbegin)
				{
					rbegin->~Content();
				}
			}

			_count -= count;
		}

		void             insert(pair<Key, Value>);
		void             insert(pair<Key, unique_ptr<PtrType>>);
		void             append(pair<Key, Value>);
		void             append(pair<Key, unique_ptr<PtrType>>);
		void             receive(TailAppendWay, Elements&&);
		void             receive(HeadInsertWay, Elements&&);
		void             receive(HeadInsertWay, uint16_t, Elements&);
		void             receive(TailAppendWay, uint16_t, Elements&);
		uint16_t         changeKeyOf(PtrType *, Key);
		pair<Key, Value> exchangeMax(pair<Key, Value>);
		pair<Key, Value> exchangeMin(pair<Key, Value>, bool &maxChanged);
		pair<Key, unique_ptr<PtrType>> exchangeMax(pair<Key, unique_ptr<PtrType>>);
		pair<Key, unique_ptr<PtrType>> exchangeMin(pair<Key, unique_ptr<PtrType>>, bool &maxChanged);


		ValueForContent& operator[](const Key& key)
		{
			return const_cast<ValueForContent&>(
				(static_cast<const Elements&>(*this))[key]
				);
		}

		Content& operator[](uint16_t i)
		{
			return const_cast<Content&>(
				(static_cast<const Elements&>(*this))[i]
				);
		}

		const ValueForContent& operator[](const Key& key) const
		{
			auto i = indexOf(key);

			if (i != -1)
			{
				return _elements[i].second;
			}

			throw runtime_error("Can't get the Value correspond"
				+ key
				+ ","
				+ " Please check the key existence.");
		}

		const Content& operator[](uint16_t i) const
		{
			return _elements[i];
		}

		int32_t indexOf(const PtrType* pointer) const
		{
#define PTR_OF_ELE ptr(_elements[i].second)

			for (auto i = 0; i < _count; ++i)
			{
				if (PTR_OF_ELE == pointer)
				{
					return i;
				}
			}

			return -1; // means not found
#undef PTR_OF_ELE
		}

		int32_t indexOf(const Key& key) const
		{
#define KEY_OF_ELE _elements[i].first
			auto& lessThan = *LessThanPtr;

			for (auto i = 0; i < _count; ++i)
			{
				if (lessThan(key, KEY_OF_ELE) == lessThan(KEY_OF_ELE, key))
				{
					return i;
				}
			}

			return -1; // means not found
#undef KEY_OF_ELE
		}

		uint16_t suitPosition(const Key& key) const
		{
#define KEY_OF_ELE _elements[i].first
			auto& lessThan = *LessThanPtr;

			for (auto i = 0; i < _count; ++i)
			{
				if (lessThan(key, KEY_OF_ELE))
				{
					return i;
				}
			}

			return _count;
#undef KEY_OF_ELE
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

		static Value& value_Ref(ValueForContent &v)
		{
			return std::get<Value>(v);
		}

		static const Value& value_Ref(const ValueForContent &v)
		{
			return std::get<Value>(v);
		}

		static Value value_Copy(const ValueForContent& v)
		{
			return std::get<Value>(v);
		}

		static Value value_Move(ValueForContent& v)
		{
			return std::move(std::get<Value>(v));
		}

		static PtrType* ptr(const ValueForContent& v)
		{
			return std::get<unique_ptr<PtrType>>(v).get();
		}

	private:
		uint16_t                   _count;
		array<Content, BtreeOrder> _elements;

		template <typename T>
		pair<Key, T> exchangeMax(pair<Key, T>);
		template <typename T>
		pair<Key, T> exchangeMin(pair<Key, T>, bool &maxChanged);
		template <typename T>
		void         append(pair<Key, T>);
		template <typename T>
		void         insert(pair<Key, T>);
		template <typename T>
		bool         add(pair<Key, T>);

		void adjustMemory(int32_t direction, uint16_t index)
		{
			moveElement(direction, begin() + index);
		}

		auto     cloneInternalElements() const;
		void     moveElement(int32_t, decltype(_elements.begin()));

		static unique_ptr<PtrType>& uniquePtr_Ref(ValueForContent& v)
		{
			return std::get<unique_ptr<PtrType>>(v);
		}

		static const unique_ptr<PtrType>& uniquePtr_Ref(const ValueForContent& v)
		{
			return std::get<unique_ptr<PtrType>>(v);
		}

		static unique_ptr<PtrType> uniquePtr_Move(ValueForContent& v)
		{
			return std::move(uniquePtr_Ref(v));
		}
	};
}

namespace Collections {
#define ELE Elements<Key, Value, BtreeOrder, PtrType>

#define VOID_RET_MODIFY_METHOD_INSTANCE(METHOD, T) ELEMENTS_TEMPLATE void ELE::METHOD(pair<Key, T> p) { return METHOD<T>(std::move(p)); }

	VOID_RET_MODIFY_METHOD_INSTANCE(insert, Value)
		VOID_RET_MODIFY_METHOD_INSTANCE(insert, unique_ptr<PtrType>)

		ELEMENTS_TEMPLATE
		template <typename T>
	void
		ELE::insert(pair<Key, T> p)
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
		adjustMemory(1, i);
		_elements[i] = std::move(p);
		++_count;
	}

	VOID_RET_MODIFY_METHOD_INSTANCE(append, Value)
		VOID_RET_MODIFY_METHOD_INSTANCE(append, unique_ptr<PtrType>)

#undef VOID_RET_MODIFY_METHOD_INSTANCE
		ELEMENTS_TEMPLATE
		template <typename T>
	void
		ELE::append(pair<Key, T> p)
	{
		if (full())
		{
			throw runtime_error("No free space to add");
		}

		_elements[_count] = std::move(p);
		++_count;
	}

	ELEMENTS_TEMPLATE
		void
		ELE::receive(TailAppendWay, Elements&& that)
	{
		receive(TailAppendWay(), that.count(), that);
	}

	ELEMENTS_TEMPLATE
		void
		ELE::receive(HeadInsertWay, Elements&& that)
	{
		receive(HeadInsertWay(), that.count(), that);
	}

	ELEMENTS_TEMPLATE
		void
		ELE::receive(HeadInsertWay, uint16_t count, Elements& that)
	{
		adjustMemory(count, count); // TODO check this work right
		decltype(that._elements.begin()) start = (that._elements.end() - count);
		auto end = start + count;

		for (auto i = 0; start != end; ++start, ++i)
		{
			_elements[i] = std::move(*start);
			++_count;
		}

		that.removeItems<false>(count); // will decrease preThat _count
	}

	// start "that" where is not clear
	ELEMENTS_TEMPLATE
		void
		ELE::receive(TailAppendWay, uint16_t count, Elements& that)
	{
		for (auto i = 0; i < count; ++i)
		{
			_elements[_count] = std::move(that._elements[i]);
			++_count;
		}

		that.removeItems<true>(count);
	}

	/**
	 * @return matched index
	 */
	ELEMENTS_TEMPLATE
		uint16_t
		ELE::changeKeyOf(PtrType *ptr, Key newKey)
	{
		auto index = indexOf(ptr);
		_elements[index].first = std::move(newKey);

		return index;
	}

#ifdef BTREE_DEBUG
#define BOUND_CHECK                                                                                              \
	if (_count < BtreeOrder) {                                                                                   \
		throw runtime_error("Please invoke exchangeMax when the Elements is full, you can use full to check it");\
	} else if (have(p.first)) {                                                                                  \
		throw runtime_error("The Key has already existed");                                     \
	}
#else
#define BOUND_CHECK
#endif


#define EXCHANGE_MAX_INSTANCE(T) ELEMENTS_TEMPLATE pair<Key, T> ELE::exchangeMax(pair<Key, T> p) { return exchangeMax<T>(std::move(p)); }

	EXCHANGE_MAX_INSTANCE(Value)
		EXCHANGE_MAX_INSTANCE(unique_ptr<PtrType>)

#undef EXCHANGE_MAX_INSTANCE

		ELEMENTS_TEMPLATE
		template <typename T>
	pair<Key, T>
		ELE::exchangeMax(pair<Key, T> p)
	{
		BOUND_CHECK

			auto& maxItem = _elements[_count - 1];
		auto key = maxItem.first;
		auto valueForContent = std::move(maxItem.second);

		--_count;
		add(std::move(p));

		if constexpr (std::is_same<T, Value>::value)
		{
			return make_pair<Key, T>(std::move(key), value_Move(valueForContent));
		}
		else
		{
			return make_pair<Key, T>(std::move(key), uniquePtr_Move(valueForContent));
		}
	}
#define EXCHANGE_MIN_INSTANCE(T) ELEMENTS_TEMPLATE pair<Key, T> ELE::exchangeMin(pair<Key, T> p, bool &maxChanged) { return exchangeMin<T>(std::move(p), maxChanged); }

	EXCHANGE_MIN_INSTANCE(Value)
		EXCHANGE_MIN_INSTANCE(unique_ptr<PtrType>)

#undef EXCHANGE_MIN_INSTANCE

		ELEMENTS_TEMPLATE
		template <typename T>
	pair<Key, T>
		ELE::exchangeMin(pair<Key, T> p, bool &maxChanged)
	{
		BOUND_CHECK

			auto& minItem = _elements[0];
		auto key = std::move(minItem.first);
		auto valueForContent = std::move(minItem.second);

		// move left
		adjustMemory(-1, 1);
		--_count;

		maxChanged = add(std::move(p));

		if constexpr (std::is_same<T, Value>::value) {
			return make_pair<Key, T>(std::move(key), std::move(value_Ref(valueForContent)));
		}
		else
		{
			return make_pair<Key, T>(std::move(key), uniquePtr_Move(valueForContent));
		}
	}

#ifdef BOUND_CHECK
#undef BOUND_CHECK
#endif

	/**
	 * @return max changed or not
	 */
	ELEMENTS_TEMPLATE
		template <typename T>
	bool
		ELE::add(pair<Key, T> p)
	{
		auto& lessThan = *LessThanPtr;

		if (lessThan(_elements[_count - 1].first, p.first))
		{
			append(std::move(p));
			return true;
		}
		else
		{
			insert(std::move(p));
			return false;
		}
	}

	ELEMENTS_TEMPLATE
		auto
		ELE::cloneInternalElements() const
	{
		decltype(_elements) es;

		for (auto i = 0; i < _count; ++i)
		{
			es[i].first = _elements[i].first;
			if (!MiddleFlag)
			{
				es[i].second = value_Ref(_elements[i].second);
			}
			else
			{
				es[i].second = std::move(uniquePtr_Ref(_elements[i].second)->clone());
			}
		}

		return std::move(es);
	}

	/**
	 * start included, still exist
	 */
	ELEMENTS_TEMPLATE
		void
		ELE::moveElement(int32_t direction, decltype(_elements.begin()) start)
	{
		if (direction < 0)
		{
			auto end = this->end();

			for (auto& begin = start; begin != end; ++begin)
			{
				*(begin + direction) = std::move(*begin);
			}
		}
		else if (direction > 0)
		{
			decltype(_elements.rend()) rend{ start - 1 };

			for (auto rbegin = _elements.rbegin(); rbegin != rend; ++rbegin)
			{
				*(rbegin + direction) = std::move(*rbegin);
			}
		}
	}
}
#undef ELE
#undef ELEMENTS_TEMPLATE