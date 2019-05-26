#pragma once
#include <functional> // for function
#include <variant>    // variant
#include <cstring>    // for memcpy
#include <exception>  // for runtime_error
#include <string>     // for to_string
#include <vector>     // for vector
#include <memory>     // for unique_ptr, shared_ptr
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

#define ELEMENTS_TEMPLATE template <typename Key, typename Value, uint16_t BtreeOrder, typename PtrType>

	ELEMENTS_TEMPLATE
	class Elements {
	public:
		using ValueForContent = variant<Value, unique_ptr<PtrType>>;
		using Content     = pair<Key, ValueForContent>;
		using CompareFunc = function<bool(const Key&, const Key&)>;
		const bool LeafFlag;

		template <typename Iter>
		Elements(Iter, Iter, shared_ptr<CompareFunc>);
		Elements(const Elements&);
		Elements(Elements&&) noexcept;

		// bool means max key change
		Key         rightMostKey() const;
		bool        have(const Key&);
		vector<Key> allKey() const;
		uint16_t    count() const;
		bool        full() const;
		bool        remove(const Key&);
		template <typename T>
		bool        insert(pair<Key, T>);
		template <typename T>
		bool        append(pair<Key, T>);

		// should provide reference?
		ValueForContent& operator[](const Key&);
		Content& operator[](uint16_t);

		pair<Key, Value> exchangeMax(pair<Key, Value>);
		PtrType* ptrOfMin() const; // for PtrType for Btree traverse all leaf
		PtrType* ptrOfMax() const; // for add the key beyond the max bound

		static Value&   value(ValueForContent&);
		static PtrType* ptr(const ValueForContent&);

	private:
		shared_ptr<CompareFunc>    _compareFuncPtr;
		CompareFunc                _compareFunc;
		uint16_t                   _count{ 0 };
		uint16_t                   _cacheIndex{ 0 };
		array<Content, BtreeOrder> _elements;

		void     adjustMemory(int16_t , Content*);
		uint16_t relatedIndex(const Key&);

		static Content& assign(Content&, pair<Key, PtrType*>);
		static Content& assign(Content&, pair<Key, Value>);
		static Content* moveElement(int16_t, Content*, Content*);
		static void initialInternalElements(array<Content, BtreeOrder>&, const array<Content, BtreeOrder>&, bool, uint16_t);
	};
}

namespace btree {
#define ELE Elements<Key, Value, BtreeOrder, PtrType>
	// public method part:

	ELEMENTS_TEMPLATE
	template <typename Iter>
	ELE::Elements(
		Iter begin,
		Iter end,
		shared_ptr<CompareFunc> funcPtr
	) :
		LeafFlag(std::is_same<typename std::decay<decltype(*begin)>::type, pair<Key, Value>>::value),
		_compareFuncPtr(funcPtr),
		_compareFunc(*_compareFuncPtr)
	{
		if (begin == end) {
			return;
		} else {
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
		_compareFuncPtr(that._compareFuncPtr),
		_compareFunc(*_compareFuncPtr),
		_count(that._count)
	{
		initialInternalElements(_elements, that._elements, that.LeafFlag, _count);
	}

	ELEMENTS_TEMPLATE
	ELE::Elements(Elements&& that) noexcept :
		LeafFlag(that.LeafFlag),
		_compareFuncPtr(that._compareFuncPtr),
		_compareFunc(*_compareFuncPtr),
		_count(that._count),
		_elements(std::move(that._elements))
	{
		that._count = 0;
	}

	ELEMENTS_TEMPLATE
	Key
	ELE::rightMostKey() const
	{
		return _elements[_count - 1].first;
	}

	ELEMENTS_TEMPLATE
	bool
	ELE::have(const Key& key)
	{
		for (auto i = 0; i < _count; ++i) {
			if (_compareFunc(_elements[i].first, key) == _compareFunc(key, _elements[i].first)) {
				_cacheIndex = i;
				return true;
			}
		}
		return false;
	}

	ELEMENTS_TEMPLATE
	vector<Key>
	ELE::allKey() const
	{
		vector<Key> r;
		r.reserve(_count);
		for (size_t i = 0; i < _count; ++i) {
			r.emplace_back(_elements[i].first);
		}
		return r;
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

	// for Value
	ELEMENTS_TEMPLATE
	typename ELE::ValueForContent&
	ELE::operator[](const Key& key)
	{
		if (key == _elements[_cacheIndex].first) {
			// where to use this function, why just only need to return ptr?
			return _elements[_cacheIndex].second;
		}
		for (auto i = 0; i < _count; ++i) {
			if (key == _elements[i].first) {
				return _elements[i].second;
			}
		}

		throw runtime_error("Can't get the Value corresponding to the Key: " + key + ","
							+ " Please check the key existence.");
	}

	ELEMENTS_TEMPLATE
	typename ELE::Content&
	ELE::operator[](uint16_t i)
	{
		return _elements[i];
	}

	ELEMENTS_TEMPLATE
	template <typename T>
	bool
	ELE::insert(pair<Key, T> p)
	{
		// maybe only leaf add logic use this Elements method
		// middle add logic is in middle Node self
		auto& k = p.first;
		auto& v = p.second;

		int16_t i = 0;
		for (i = 0; i < _count; ++i) {
			if (_compareFunc(k, _elements[i].first) == _compareFunc(_elements[i].first, k)) {
				throw runtime_error("The inserting key duplicates: " + k);
			} else if (_compareFunc(_elements[i].first, k)) {
				goto Add;
			}
		}
		// when equal and bigger than the bound, throw the error
		throw runtime_error("\nWrong adding Key-Value: " + k + " " + v + ". "
							+ "Now the count of this Elements: " + std::to_string(_count )+ ". "
							+ "And please check the max Key to ensure not beyond the bound.");

		Add:
		adjustMemory(1, &_elements[i]);
		_elements[i] = std::move(p); //	Elements::assign(_elements[i], p);
		++_count;
		return false; // always false
	}

	ELEMENTS_TEMPLATE
	template <typename T>
	bool
	ELE::append(pair<Key, T> p)
	{
		if (full()) {
			throw runtime_error("No free space to append");
		}

		_elements[_count] = std::move(p);
		++_count;
		return true;
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
		if (_compareFunc(p.first, _elements[_count - 1].first)) {
			append(p);
		} else {
			insert(p);
		}

		return max;
	}

	// for ptr
	ELEMENTS_TEMPLATE
	PtrType*
	ELE::ptrOfMin() const
	{
		return Elements::ptr(_elements[0].second);
	}

	ELEMENTS_TEMPLATE
	PtrType*
	ELE::ptrOfMax() const
	{
		return Elements::ptr(_elements[_count - 1].second);
	}

	// private method part:
	ELEMENTS_TEMPLATE
	void
	ELE::adjustMemory(const int16_t direction, Content *begin)
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
#ifdef FALSE
		std::cout << "begin: " << begin << std::endl;
		std::cout << "end: " << end << std::endl;
		std::cout << "content: " << sizeof(Content) << std::endl;
#endif

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
		array<Content, BtreeOrder>& thisInternalElements,
		const array<typename ELE::Content, BtreeOrder>& thatInternalElements,
		bool valueEle,
		uint16_t count
	)
	{
		auto src = &thatInternalElements;
		auto des = &thisInternalElements;

		memcpy(des, src, count * sizeof(Content)); // copy all include unique_ptr

		if (valueEle) {
			return;
		} else {
			for (auto& e : thisInternalElements) {
				auto& ptr = std::get<unique_ptr<PtrType>>(e.second);
				auto deepClone = make_unique<PtrType>(*ptr);
				ptr.release();
				ptr.reset(deepClone.release());
			}
		}
	}

	ELEMENTS_TEMPLATE
	typename ELE::Content&
	ELE::assign(Content& ele, pair<Key, PtrType*> p)
	{
		ele.first = p.first;
		unique_ptr<PtrType> uni_ptr(p.second);

		new (&(ele.second)) variant<Value, unique_ptr<PtrType>>(std::move(uni_ptr));
		return ele;
	}

	ELEMENTS_TEMPLATE
	typename ELE::Content&
	ELE::assign(Content& ele, pair<Key, Value> p)
	{
		ele = std::move(p);
		return ele;
	}

	ELEMENTS_TEMPLATE
	Value&
	ELE::value(ValueForContent& v)
	{
		return std::get<Value>(v);
	}

	ELEMENTS_TEMPLATE
	PtrType*
	ELE::ptr(const ValueForContent& v)
	{
		return std::get<unique_ptr<PtrType>>(v).get();
	}

#undef ELE
#undef ELEMENTS_TEMPLATE
}