#pragma once
#include <functional> // for function
#include <variant>    // variant
#include <cstring>    // for memcpy
#include <exception>  // for runtime_error
#include <string>     // for to_string
#include <vector>     // for vector
#include <memory>     // for unique_ptr, shared_ptr
#include <iterator>
#ifdef BTREE_DEBUG
#include "Utility.hpp"
#endif

namespace btree {
	using std::variant;
	using std::pair;
	using std::vector;
	using std::function;
	using std::unique_ptr;
	using std::shared_ptr;
	using std::array;
	using std::memcpy;
	using std::runtime_error;
	using std::make_unique;
	using std::iterator;
	using std::forward_iterator_tag;

#define ELEMENTS_TEMPLATE template <typename Key, typename Value, uint16_t BtreeOrder, typename PtrType>

	/**
	 * First and second is to public
	 */
	ELEMENTS_TEMPLATE
	class Elements {
	public:
		class ElementsIterator;
		using ValueForContent = variant<Value, unique_ptr<PtrType>>;
		using Content         = pair<Key, ValueForContent>;
		using LessThan        = function<bool(const Key&, const Key&)>;

		const bool           LeafFlag;
		shared_ptr<LessThan> LessThanPtr;

		template <typename Iter>
		Elements(Iter, Iter, shared_ptr<LessThan>);
		Elements(const Elements&);
		Elements(Elements&&) noexcept;

		// Bool means max key changes
		bool             have(const Key&);
		uint16_t         count() const;
		bool             full() const;
		bool             remove(const Key&);
		template <typename T>
		void             insert(pair<Key, T>);
		template <typename T>
		void             append(pair<Key, T>);
        pair<Key, Value> exchangeMax(pair<Key, Value>);

		ValueForContent& operator[](const Key&);
		Content&         operator[](uint16_t);
		ElementsIterator       begin();
		ElementsIterator       end();

		static Value&         value(ValueForContent&);
		static PtrType*       ptr  (ValueForContent&);
        static const Value&   value(const ValueForContent&);
        static const PtrType* ptr  (const ValueForContent&);

	private:
		uint16_t                   _count{ 0 };
		array<Content, BtreeOrder> _elements;

		void     adjustMemory(int16_t , Content*);
		uint16_t relatedIndex(const Key&);

		static Content& assign(Content&, const pair<Key, Value>&);
		template <typename T>
		static Content& assign(Content&, pair<Key, unique_ptr<T>>&);
		static Content* moveElement(int16_t, Content*, Content*);
		static void initialInternalElements(array<Content, BtreeOrder>&, const array<Content, BtreeOrder>&, bool, uint16_t);
		static unique_ptr<PtrType>& uniquePtr(ValueForContent&);

	public:
		class ElementsIterator : iterator<forward_iterator_tag, Content> {
			Content* _ptr;

		public:
			explicit ElementsIterator(Content* p) : _ptr(p) {}

			ElementsIterator& operator->()       { _ptr = ++_ptr; return *this; }
			Content&          operator* () const { return *_ptr; }
			Content*          operator->() const { return _ptr; }
			ElementsIterator& operator++()       { ++_ptr; return *this; }
			bool              operator==(const ElementsIterator& i) const
			{ return _ptr == i._ptr; }
			bool              operator!=(const ElementsIterator& i) const
			{ return _ptr != i._ptr; }
		};
	};
}

namespace btree {
#define ELE Elements<Key, Value, BtreeOrder, PtrType>

	ELEMENTS_TEMPLATE
	template <typename Iter>
	ELE::Elements(
		Iter begin,
		Iter end,
		shared_ptr<LessThan> lessThanPtr
	) :
		LeafFlag(std::is_same<typename std::decay<decltype(*begin)>::type, pair<Key, Value>>::value),
		LessThanPtr(lessThanPtr)
	{
		if (begin != end) {
			do {
				Elements::assign(_elements[_count], *begin);

				++_count;
				++begin;
			} while (begin != end);
		}
	}

	ELEMENTS_TEMPLATE
	ELE::Elements(const Elements& that) :
		LeafFlag(that.LeafFlag),
		LessThanPtr(that.LessThanPtr),
		_count(that._count)
	{
		initialInternalElements(_elements, that._elements, that.LeafFlag, _count);
	}

	ELEMENTS_TEMPLATE
	ELE::Elements(Elements&& that) noexcept :
		LeafFlag(that.LeafFlag),
		LessThanPtr(that.LessThanPtr),
		_count(that._count),
		_elements(std::move(that._elements))
	{
		that._count = 0;
	}

	ELEMENTS_TEMPLATE
	bool
	ELE::have(const Key& key)
	{
		for (auto i = 0; i < _count; ++i) {
			if ((*LessThanPtr)(_elements[i].first, key) == (*LessThanPtr)(key, _elements[i].first)) {
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
	typename ELE::ValueForContent&
	ELE::operator[](const Key& key)
	{
		for (auto i = 0; i < _count; ++i) {
			if (key == _elements[i].first) {
				return _elements[i].second;
			}
		}

		throw runtime_error("Can't get the Value corresponding to the Key: "
                            + key
                            + ","
							+ " Please check the key existence.");
	}

	ELEMENTS_TEMPLATE
	typename ELE::Content&
	ELE::operator[](uint16_t i)
	{
		return _elements[i];
	}

	ELEMENTS_TEMPLATE
	typename ELE::ElementsIterator
	ELE::begin()
	{
		return ElementsIterator(_elements.begin());
	}

	ELEMENTS_TEMPLATE
	typename ELE::ElementsIterator
	ELE::end()
	{
		return ElementsIterator(_elements.end());
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
		throw runtime_error("\nWrong adding Key-Value: " + k + " " + v + ". "
							+ "Now the count of this Elements: " + std::to_string(_count )+ ". "
							+ "And please check the max Key to ensure not beyond the bound.");

		Insert:
		adjustMemory(1, &_elements[i]);
		_elements[i] = std::move(p); //	Elements::assign(_elements[i], p);
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

		_elements[_count] = std::move(p);
		++_count;
	}

	ELEMENTS_TEMPLATE
	pair<Key, Value>
	ELE::exchangeMax(pair<Key, Value> p)
	{
#ifdef BTREE_DEBUG
		if (_count < BtreeOrder) {
			throw runtime_error("Please invoke exchangeMax when the Elements is full, you can use full to check it");
		} else if (have(p.first)) {
			throw runtime_error("The Key: " + p.first + " has already existed");
		}
#endif
		auto& maxItem = _elements[_count - 1];
		pair<Key, Value> max{ maxItem.first, Elements::value(maxItem.second) };
		--_count;
		if ((*LessThanPtr)(_elements[_count - 1].first, p.first)) {
			append(p);
		} else {
			insert(p);
		}

		return max;
	}

	// private method part:
	ELEMENTS_TEMPLATE
	void
	ELE::adjustMemory(const int16_t direction, Content* begin)
	{
		// default Content* end
		if (_count == 0 || direction == 0) {
			return;
		} else if (direction > 0 && _count >= BtreeOrder) {
			throw runtime_error("The Elements is full");
		}
		auto&& end = _count >= BtreeOrder ? _elements.end() : &_elements[_count]; // end is exclude
		Elements::moveElement(direction, begin, end);
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

// static method implementation
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
	ELE::initialInternalElements(
		array<Content, BtreeOrder>& thisElements,
		const array<typename ELE::Content, BtreeOrder>& thatElements,
		bool isValue,
		uint16_t count
	)
	{
		auto src = &thatElements;
		auto des = &thisElements;

		memcpy(des, src, count * sizeof(Content)); // copy all without distinguish

		if (!isValue) {
			for (auto& e : thisElements) {
				auto& ptr = std::get<unique_ptr<PtrType>>(e.second);
				auto deepClone = ptr->clone();
				ptr.release();
				ptr.reset(deepClone.release());
			}
		}
	}

	ELEMENTS_TEMPLATE
    template <typename T>
	typename ELE::Content&
	ELE::assign(Content &ele, pair<Key, unique_ptr<T>>& ptrPair)
	{
        static_assert(std::is_base_of<PtrType, T>::value, "The type to be stored should be derived from PtrType");

		ele.first = ptrPair.first;
		ELE::uniquePtr(ele.second).reset(ptrPair.second.release());
		// ele.second = std::move(make_unique<PtrType>(ptrPair.second.release()));
		// new (&(ele.second)) ValueForContent(std::move(ptrPair.second));
		return ele;
	}

	ELEMENTS_TEMPLATE
	typename ELE::Content&
	ELE::assign(Content &ele, const pair<Key, Value>& pairPtr)
	{
		ele = std::move(pairPtr);
		return ele;
	}

	ELEMENTS_TEMPLATE
	Value&
	ELE::value(ValueForContent& v)
	{
		return std::get<Value>(v);
	}

	ELEMENTS_TEMPLATE
	const Value&
	ELE::value(const ValueForContent& v)
	{
		return std::get<Value>(v);
	}

	ELEMENTS_TEMPLATE
	PtrType*
	ELE::ptr(ValueForContent& v)
	{
		return std::get<unique_ptr<PtrType>>(v).get();
	}

    ELEMENTS_TEMPLATE
    const PtrType*
	ELE::ptr(const ValueForContent& v)
	{
		return std::get<unique_ptr<PtrType>>(v).get();
	}

	ELEMENTS_TEMPLATE
    unique_ptr<PtrType>&
	ELE::uniquePtr(ValueForContent& v)
    {
	    return std::get<unique_ptr<PtrType>>(v);
	}
#undef ELE
#undef ELEMENTS_TEMPLATE
}