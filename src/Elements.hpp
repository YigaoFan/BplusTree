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

namespace btree {
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

	/**
	 * First and second relationship is to public
	 */
	ELEMENTS_TEMPLATE
	class Elements {
	public:
		using ValueForContent = variant<Value, unique_ptr<PtrType>>;
		using Content         = pair<Key, ValueForContent>;
		using LessThan        = function<bool(const Key&, const Key&)>;

		const bool           LeafFlag;
		shared_ptr<LessThan> LessThanPtr;

		template <typename Iter>
		Elements(Iter, Iter, shared_ptr<LessThan>);
		Elements(const Elements&);
		Elements(Elements&&) noexcept;

		bool             have(const Key&) const;
		uint16_t         count() const;
		bool             full()  const;

		// bool means max key changes
		bool             remove(const Key&);
		bool             remove(uint16_t); // TODO maybe will implement
		void             removeItemsFrom(bool, uint16_t);
		template <typename T>
		void             insert(pair<Key, T>);
		template <typename T>
		void             append(pair<Key, T>);
		template <typename T>
		pair<Key, T> exchangeMax(pair<Key, T>);
		template <typename T>
		pair<Key, T> exchangeMin(pair<Key, T>);
		// TODO maybe directly use PtrType without * this project every where

		ValueForContent& operator[](const Key&);
		Content&         operator[](uint16_t);
		const ValueForContent& operator[](const Key&) const;
		const Content&         operator[](uint16_t)   const;

		int32_t  indexOf         (const PtrType*) const;
		int32_t  indexOf         (const Key&)     const;
		uint16_t suitablePosition(const Key&)     const;
		auto     begin();
		auto     end  ();
		auto     begin() const;
		auto     end  () const;

		//TODO should use reference or value? actually is read way and write way and copy way
		static Value&         value_Ref(ValueForContent&);
		static const Value&   value_Ref(const ValueForContent&);
		static Value          value_Copy(const ValueForContent&);
		static Value          value_Move(ValueForContent&);
		static PtrType*       ptr  (ValueForContent&);
		static PtrType*       ptr  (const ValueForContent&);

	private:
		uint16_t                   _count;
		array<Content, BtreeOrder> _elements;

		void     adjustMemory(int32_t, Content *);
		uint16_t relatedIndex(const Key&);
		template <typename T>
		void     add(pair<Key, T>);
		auto cloneInternalElements() const;

		static void assign(Content&, pair<Key, Value>&);
		static void assign(Content&, pair<Key, unique_ptr<PtrType>>&);
		static Content* moveElement(int16_t, Content*, Content*);

		static unique_ptr<PtrType>& uniquePtr_Ref (ValueForContent&);
		static unique_ptr<PtrType>  uniquePtr_Move(ValueForContent&);
	};
}

namespace btree {
#define ELE Elements<Key, Value, BtreeOrder, PtrType>

	// some are copy, some are not copy
	ELEMENTS_TEMPLATE
	template <typename Iter>
	ELE::Elements(
		Iter begin,
		Iter end,
		shared_ptr<LessThan> lessThanPtr
	) :
		LeafFlag(std::is_same<typename std::decay<decltype(*begin)>::type, pair<Key, Value>>::value),
		LessThanPtr(lessThanPtr),
		_count(0)
	{
		do {
			Elements::assign(_elements[_count], *begin);

			++_count;
			++begin;
		} while (begin != end);
	}

	ELEMENTS_TEMPLATE
	ELE::Elements(const Elements& that) :
		LeafFlag   (that.LeafFlag),
		LessThanPtr(that.LessThanPtr),
		_elements  (that.cloneInternalElements()),
		_count     (that._count)
	{ }

	ELEMENTS_TEMPLATE
	ELE::Elements(Elements&& that) noexcept :
		LeafFlag   (that.LeafFlag),
		LessThanPtr(that.LessThanPtr),
		_elements  (std::move(that._elements)),
		_count     (that._count)
	{
		that._count = 0;
	}

	ELEMENTS_TEMPLATE
	bool
	ELE::have(const Key& key) const
	{
		auto& lessThan = *LessThanPtr;
		for (auto i = 0; i < _count; ++i) {
			auto& eleKey = _elements[i].first;
			if (lessThan(eleKey, key) == lessThan(key, eleKey)) {
				return true;
			}
		}
		return false;
	}

	ELEMENTS_TEMPLATE
	uint16_t
	ELE::count() const
	{
		return _count;
	}

	ELEMENTS_TEMPLATE
	bool
	ELE::Elements::full() const
	{
		return _count >= BtreeOrder;
	}

	ELEMENTS_TEMPLATE
	bool
	ELE::remove(const Key& key)
	{
		auto i = relatedIndex(key);
		auto boundChanged{ false };

		adjustMemory(-1, &_elements[i + 1]);
		if (i == (_count - 1)) {
			boundChanged = true;
		}
		--_count;
		return boundChanged;
	}

	ELEMENTS_TEMPLATE
	void
	ELE::removeItemsFrom(bool head, uint16_t count)
	{
		if (head) {
			adjustMemory(-count, &_elements[0]);
		}

		_count -= count;
	}

	ELEMENTS_TEMPLATE
	typename ELE::ValueForContent&
	ELE::operator[](const Key& key)
	{
		return const_cast<ValueForContent&>(
			(static_cast<const Elements&>(*this))[key]
		);
	}

	ELEMENTS_TEMPLATE
	const typename ELE::ValueForContent&
	ELE::operator[](const Key& key) const
	{
		auto i = indexOf(key);
	
		if (i != -1) {
			return _elements[i].second;
		}
    
		throw runtime_error("Can't get the Value correspond"
							 + key
							 + ","
							 + " Please check the key existence.");
	}

	ELEMENTS_TEMPLATE
	typename ELE::Content&
	ELE::operator[](uint16_t i)
	{
		return const_cast<Content&>(
			(static_cast<const Elements&>(*this))[i]
			);
	}

	ELEMENTS_TEMPLATE
	const typename ELE::Content&
	ELE::operator[](uint16_t i) const
	{
		return _elements[i];
	}

	ELEMENTS_TEMPLATE
	auto
	ELE::begin()
	{
		return _elements.begin();
	}

	ELEMENTS_TEMPLATE
	auto
	ELE::end()
	{
		return _elements.end();
	}

	ELEMENTS_TEMPLATE
	auto
	ELE::begin() const
	{
		return _elements.begin();
	}

	ELEMENTS_TEMPLATE
	auto
	ELE::end() const
	{
		return _elements.end();
	}

	ELEMENTS_TEMPLATE
	int32_t
	ELE::indexOf(const PtrType* pointer) const
	{
#define PTR_OF_ELE ptr(_elements[i].second)

		for (auto i = 0; i < _count; ++i) {
			if (PTR_OF_ELE == pointer) {
				return i;
			}
		}

		return -1; // means not found
#undef PTR_OF_ELE
	}


	ELEMENTS_TEMPLATE
	int32_t
	ELE::indexOf(const Key& key) const
	{
#define KEY_OF_ELE _elements[i].first
		auto& lessThan = *LessThanPtr;

		for (auto i = 0; i < _count; ++i) {
			if ((!lessThan(key, KEY_OF_ELE)) && (!lessThan(KEY_OF_ELE, key))) {
				return i;
			}
		}

		return -1; // means not found
#undef KEY_OF_ELE
	}

	ELEMENTS_TEMPLATE
	uint16_t
	ELE::suitablePosition(const Key& key) const
	{
#define KEY_OF_ELE _elements[i].first
		auto& lessThan = *LessThanPtr;

		for (auto i = 0; i < _count; ++i) {
			if (lessThan(key, KEY_OF_ELE)) {
				return i;
			}
		}

		return _count + 1;
#undef KEY_OF_ELE
	}

	ELEMENTS_TEMPLATE
	template <typename T>
	void
	ELE::insert(pair<Key, T> p)
	{
		// maybe only leaf add logic use this Elements method
		// middle add logic is in middle Node self
		auto& k = p.first;
		auto& v = p.second;

		int16_t i = 0;
		for (; i < _count; ++i) {
			if ((*LessThanPtr)(k, _elements[i].first) == (*LessThanPtr)(_elements[i].first, k)) {
				throw runtime_error("The inserting key duplicates: " + k);
			} else if ((*LessThanPtr)(k, _elements[i].first)) {
				goto Insert;
			}
		}
		// when equal and bigger than the bound, throw the error
		throw runtime_error(/*"\nWrong adding Key-Value: " + k + " " + v + ". "
							+ "Now the count of this Elements: " + std::to_string(_count)+ ". "
							+ */"And please check the max Key to ensure not beyond the bound.");

		Insert:
		adjustMemory(1, &_elements[i]);
		Elements::assign(_elements[i], p);
		++_count;
	}

	ELEMENTS_TEMPLATE
	template <typename T>
	void
	ELE::append(pair<Key, T> p)
	{
		if (full()) {
			throw runtime_error("No free space to add");
		}

		Elements::assign(_elements[_count], p);
		++_count;
	}

#ifdef BTREE_DEBUG
#define BOUND_CHECK                                                                                              \
	if (_count < BtreeOrder) {                                                                                   \
		throw runtime_error("Please invoke exchangeMax when the Elements is full, you can use full to check it");\
	} else if (have(p.first)) {                                                                                  \
		throw runtime_error("The Key: " + p.first + " has already existed");                                     \
	}
#endif

	ELEMENTS_TEMPLATE
	template <typename T>
	pair<Key, T>
	ELE::exchangeMax(pair<Key, T> p)
	{
#ifdef BTREE_DEBUG
		BOUND_CHECK
#endif
		auto& maxItem = _elements[_count - 1];
		auto key = maxItem.first;
		auto valueForContent = std::move(maxItem.second);

		--_count;
		add(std::move(p));
		
		if constexpr (std::is_same<T, Value>::value) {
			return make_pair<Key, T>(std::move(key), std::move(value_Ref(valueForContent)));
		} else {
			return make_pair<Key, T>(std::move(key), std::move(ptr(valueForContent)));
		}
	}

	ELEMENTS_TEMPLATE
	template <typename T>
	pair<Key, T>
	ELE::exchangeMin(pair<Key, T> p)
	{
#ifdef BTREE_DEBUG
		BOUND_CHECK
#endif
		auto& minItem = _elements[0];
		auto key = std::move(minItem.first);
		auto valueForContent = std::move(minItem.second);
		// pair<Key, Value> min{ minItem.first, value(minItem.second) };
		// move left
		adjustMemory(-1, &_elements[1]);
		--_count;

		add(p);

		if constexpr (std::is_same<T, Value>::value) {
			return make_pair<Key, T>(std::move(key), std::move(value_Ref(valueForContent)));
		} else {
			return make_pair<Key, T>(key, ptr(valueForContent));
		}
	}

#ifdef BOUND_CHECK
#undef BOUND_CHECK
#endif

	ELEMENTS_TEMPLATE
	template <typename T>
	void
	ELE::add(pair<Key, T> p)
	{
		auto& lessThan = *LessThanPtr;

		if (lessThan(_elements[_count - 1].first, p.first)) {
			append(p);
		} else {
			insert(p);
		}
	}

	ELEMENTS_TEMPLATE
	auto
	ELE::cloneInternalElements() const
	{
		decltype(_elements) eles;

		memcpy(&eles, &_elements, _count * sizeof(Content)); // copy all without distinguish

		if (LeafFlag) {
			for (auto i = 0; i < _count; ++i) {
				eles[i] = _elements[i];
			}
		} else {
			for (auto i = 0; i < _count; ++i) {
				eles[i].first = _elements[i].first;
				eles[i].second = std::move(uniquePtr_Ref(_elements[i].second)->clone());
			}
		}

		return std::move(eles);
	}
	ELEMENTS_TEMPLATE
	void
	ELE::adjustMemory(int32_t direction, Content *begin)
	{
		// default Content* end
		if (_count == 0 || direction == 0) {
			return;
		} else if (_count >= BtreeOrder && direction > 0) {
			throw runtime_error("The Elements is full");
		}
		auto end = /*(_count >= BtreeOrder) ? _elements.end() : */&_elements[_count]; // end is exclude
		moveElement(direction, begin, end);
	}

	ELEMENTS_TEMPLATE
	uint16_t
	ELE::relatedIndex(const Key &key)
	{
		for (auto i = 0; i < _count; ++i) {
			if (_elements[i].first == key) {
				return i;
			}
		}

		throw runtime_error("Can't get the related element of the key: " + key);
	}
}

namespace btree {
	ELEMENTS_TEMPLATE
	typename ELE::Content*
	ELE::moveElement(int16_t direction, Content* begin, Content* end)
	{
		// depend on different direction, where to start copy is different
		// in the future, could use WORD_BIT to copy
		if (direction < 0) {
			return static_cast<Content *>(
				memcpy(begin + direction, begin, (end - begin) * sizeof(Content)));
		} else if (direction > 0) {
			for (auto current = end - 1; current >= begin; --current) {
				return static_cast<Content *>(
					memcpy(current + direction, current, sizeof(Content)));
			}
		}

		return begin;
	}

	ELEMENTS_TEMPLATE
	void
	ELE::assign(Content& e, pair<Key, unique_ptr<PtrType>>& ptrPair)
	{
		e.first = ptrPair.first;
		ELE::uniquePtr_Ref(e.second).reset(ptrPair.second.release());
	}

	ELEMENTS_TEMPLATE
	void
	ELE::assign(Content& e, pair<Key, Value>& p)
	{
		e = std::move(p);
	}

	ELEMENTS_TEMPLATE
	Value&
	ELE::value_Ref(ValueForContent &v)
	{
		return std::get<Value>(v);
	}

	ELEMENTS_TEMPLATE
	Value
	ELE::value_Copy(const ValueForContent& v)
	{
		return std::get<Value>(v);
	}

	ELEMENTS_TEMPLATE
	Value
	ELE::value_Move(ValueForContent& v)
	{
		return std::move(std::get<Value>(v));
	}

	ELEMENTS_TEMPLATE
	const Value&
	ELE::value_Ref(const ValueForContent &v)
	{
		return std::get<const Value>(v);
	}

	ELEMENTS_TEMPLATE
	PtrType*
	ELE::ptr(ValueForContent& v)
	{
		return uniquePtr_Ref(v).get();
	}

	ELEMENTS_TEMPLATE
	PtrType*
	ELE::ptr(const ValueForContent& v)
	{
		return std::get<unique_ptr<PtrType>>(v).get();
	}

	ELEMENTS_TEMPLATE
	unique_ptr<PtrType>&
	ELE::uniquePtr_Ref(ValueForContent &v)
	{
		return std::get<unique_ptr<PtrType>>(v);
	}

	ELEMENTS_TEMPLATE
	unique_ptr<PtrType>
	ELE::uniquePtr_Move(ValueForContent& v)
	{
		return std::move(uniquePtr_Ref(v));
	}

#undef ELE
#undef ELEMENTS_TEMPLATE
}