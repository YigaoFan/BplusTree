#pragma once
#include <functional> // for function
#include <variant> // variant
#include <cstring> // for memcpy
#include <exception> // for runtime_error
#include <string> // for to_string
#include <vector>
#ifdef BTREE_DEBUG
#include "Utility.hpp"
#endif

namespace btree {
	template <typename Key, typename Value, int16_t BtreeOrder, typename NodeType>
	class Elements {
	public:
		using content_type = std::pair<Key, std::variant<Value, std::unique_ptr<NodeType>>>;

		template <typename Iterator>
		Elements(Iterator, Iterator, const std::function<bool(const Key, const Key)>&);

		Elements(const Elements&, std::function<bool(const Key, const Key)>&);
		// Common
		Key rightmostKey() const;
		bool have(const Key&);
		std::vector<Key> allKey() const;
		int16_t count() const;
		bool full() const;
		bool remove(const Key&);
		template <typename T> // use T to not limit to key-value
		bool insert(const std::pair<Key, T> &);
		template <typename T>
		bool append(const std::pair<Key, T>&);

		// all here bool means change
		std::variant<Value, std::unique_ptr<NodeType>>& operator[](const Key&);

		// for Value
		content_type squeezeOne(const std::pair<Key, Value> &); // when full call this function
		// for ptr
		NodeType* ptrOfMin() const; // for Node for Btree traverse all leaf
		NodeType* ptrOfMax() const; // for add the key beyond the max bound
		bool changeValue(const Key &, const Key &);

		static Value& value(std::variant<Value, std::unique_ptr<NodeType>>&);
		static NodeType* ptr(const std::variant<Value, std::unique_ptr<NodeType>>&);

	private:
		Elements() = default; // for construct null middle_type Node
		// Field
		const std::function<bool(const Key, const Key)>& _compare;
		int _count{ 0 };
		int _cacheIndex{ 0 };
		std::array<content_type, BtreeOrder> _elements;

		void adjustMemory(int16_t , content_type *);
		int16_t relatedIndex(const Key &);

		static content_type* moveElement(int16_t, content_type *, content_type *);
		static content_type& assign(content_type&, NodeType*);
		static content_type& assign(content_type&, const std::pair<Key, Value>&);
	};
}

// class implementation
namespace btree {
	using std::pair;
	using std::vector;
	using std::unique_ptr;
	using std::memcpy;
	using std::runtime_error;

	// public method part:

	template <typename Key, typename Value, int16_t BtreeOrder, typename NodeType>
	template <typename Iter>
	Elements<Key, Value, BtreeOrder, NodeType>::Elements(
		Iter begin,
		Iter end,
		const std::function<bool(const Key, const Key)>& function
	)
		: _compare(function)
	{
		if (begin == end) {
			return;
		} else {
			do {
				Elements::assign(_elements[_count], *begin);

				++_count;
#ifdef FALSE
				std::cout << "Btree debug:" << std::endl;
				std::cout << "Add Key: " << begin->first << std::endl;
				std::cout << "Count: " << _count << std::endl;
#endif
				++begin;
			} while (begin != end);
		}
	}

	template <typename Key, typename Value, int16_t BtreeOrder, typename NodeType>
	Elements<Key, Value, BtreeOrder, NodeType>::Elements(const Elements& that, std::function<bool(const Key, const Key)>& function)
		: _compare(function), _count(that._count)
	{
		std::memcpy(&_elements, &(that._elements), that._count * sizeof(content_type));
	}

	template <typename Key, typename Value, int16_t BtreeOrder, typename NodeType>
	Key
	Elements<Key, Value, BtreeOrder, NodeType>::rightmostKey() const
	{
		return _elements[_count - 1].first;
	}


	template <typename Key, typename Value, int16_t BtreeOrder, typename NodeType>
	bool
	Elements<Key, Value, BtreeOrder, NodeType>::have(const Key& key)
	{
		// have some problem
		for (auto i = 0; i < _count; ++i) {
			if (_compare(_elements[i].first, key) == _compare(key, _elements[i].first)) {
				_cacheIndex = i;
				return true;
			}
		}
		return false;
	}


	template <typename Key, typename Value, int16_t BtreeOrder, typename NodeType>
	vector<Key>
	Elements<Key, Value, BtreeOrder, NodeType>::allKey() const {
		vector<Key> r;
		r.reserve(_count);
		for (size_t i = 0; i < _count; ++i) {
			r.emplace_back(_elements[i].first);
		}
		return r;
	}

	template <typename Key, typename Value, int16_t BtreeOrder, typename NodeType>
	int16_t
	Elements<Key, Value, BtreeOrder, NodeType>::count() const
	{
		return _count;
	}

	template <typename Key, typename Value, int16_t BtreeOrder, typename NodeType>
	bool
	Elements<Key, Value, BtreeOrder, NodeType>::Elements::full() const
	{
		return _count >= BtreeOrder;
	}


	template <typename Key, typename Value, int16_t BtreeOrder, typename NodeType>
	bool
	Elements<Key, Value, BtreeOrder, NodeType>::remove(const Key& key)
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

	template <typename Key, typename Value, int16_t BtreeOrder, typename NodeType>
	std::variant<Value, unique_ptr<NodeType>>&
	Elements<Key, Value, BtreeOrder, NodeType>::operator[](const Key& key)
	{
		// TODO have problem
		// 1. should return ptr, check_out_digging need
//        auto&& cache_k = _elements[_cacheIndex].first;
//        auto&& cache_v = Elements::value(_elements[_cacheIndex].second);
//
//        if (key == cache_k) {
//            this->reset_cache();
//            return cache_v;
//        } else if (key < cache_k) {
//            for (auto i = 0; i < _cacheIndex; ++i) {
//                if (key == _elements[i].first) {
//                    return Elements::value(_elements[i].second);
//                }
//            }
//        } else {
//            for (int i = cache_k; i < _count; ++i) {
//                if (key == _elements[i].first) {
//                    return Elements::value(_elements[i].second);
//                }
//            }
//        }
		// lite version:
		if (key == _elements[_cacheIndex].first) {
			// where to use this function, why just only need to return ptr?
			return _elements[_cacheIndex].second;
		} // should use divide method
		for (auto i = 0; i < _count; ++i) {
			if (key == _elements[i].first) {
				return _elements[i].second;
			}
		}

		throw runtime_error("Can't get the Value corresponding to the Key: " + key + ","
							+ "Please check the key existence."
		);
	}

	// Node think first of full situation
	// return value indicate the the max-key is changed or not

	//
	template <typename Key, typename Value, int16_t BtreeOrder, typename NodeType>
	template <typename T>
	bool
	Elements<Key, Value, BtreeOrder, NodeType>::insert(const pair<Key, T> &pair)
	{
		// TODO change to return bool
		// maybe only leaf add logic use this Elements method
		// middle add logic is in middle Node self
		auto& k = pair.first;
		auto& v = pair.second;

//        if (k == cache_key_) {
//            auto& des = this->adjustMemory(1, &_elements[_cacheIndex + 1]);
//            des = pair;
//        } else if (k > cache_key_) {
//            // TODO maybe cache_key_ and _cacheIndex is not correspond
//            for (auto i = _cacheIndex + 1; i < _count; ++i) {
//                if (k == _elements[i].first) {
//                    // TODO think of dichotomy to get the index
//                    auto& des = this->adjustMemory(_elements[i], 1);
//                    des = pair;
//                }
//            }
//        } else {
//            for (auto i = 0; i < _cacheIndex; ++i) {
//                if (k == _elements[i].first) {
//                    auto& des = this->adjustMemory(_elements[i], 1);
//                    des = pair;
//                }
//            }
//        }
		// simple version:
		int16_t i = 0;
		for (i = 0; i < _count; ++i) {
			if (_compare(k, _elements[i].first) == _compare(_elements[i].first, k)) {
				throw runtime_error("The inserting key duplicates: " + k);
			} else if (_compare(_elements[i].first, k)) {
				goto Add;
			}
		}
		// when equal and bigger than the bound, throw the error
		throw runtime_error("\nWrong adding Key-Value: " + k + " " + v + ". "
							+ "Now the count of this Elements: " + std::to_string(_count )+ ", "
							+ "the max key: " + _elements[_count].first
							+ ", Do you beyond the max bound?");

		Add:
		adjustMemory(1, &_elements[i]);
		_elements[i] = pair; //	Elements::assign(_elements[i], pair);
		++_count;
		return false; // always false
	}

	template <typename Key, typename Value, int16_t BtreeOrder, typename NodeType>
	template <typename T>
	bool
	Elements<Key, Value, BtreeOrder, NodeType>::append(const pair<Key, T>& pair)
	{
		_elements[_count] = pair;
		++_count;
		return true;
	}

	template <typename Key, typename Value, int16_t BtreeOrder, typename NodeType>
	typename Elements<Key, Value, BtreeOrder, NodeType>::content_type
	Elements<Key, Value, BtreeOrder, NodeType>::squeezeOne(const pair<Key, Value> &pair)
	{
#ifdef BTREE_DEBUG
		if (_count < BtreeOrder) {
			throw runtime_error("Please invoke squeezeOne when the Elements is full, you can use full to check it");
		} else if (have(pair.first)) {
			throw runtime_error("The Key: " + pair.first + " has already existed");
		}
#endif

		std::pair<Key, Value> max{ _elements[_count - 1].first, Elements::value(_elements[_count - 1].second) };
		--_count;
		if (_compare(pair.first, _elements[_count - 1].first)) {
			append(pair);
			return max;
		} else {
			insert(pair);
			return max;
		}
	}

	// for ptr

	template <typename Key, typename Value, int16_t BtreeOrder, typename NodeType>
	NodeType*
	Elements<Key, Value, BtreeOrder, NodeType>::ptrOfMin() const
	{
		return Elements::ptr(_elements[0].second);
	}

	template <typename Key, typename Value, int16_t BtreeOrder, typename NodeType>
	NodeType*
	Elements<Key, Value, BtreeOrder, NodeType>::ptrOfMax() const
	{
		return Elements::ptr(_elements[_count - 1].second);
	}

	template <typename Key, typename Value, int16_t BtreeOrder, typename NodeType>
	bool
	Elements<Key, Value, BtreeOrder, NodeType>::changeValue(const Key &old_key, const Key &new_key)
	{
	}

	// private method part:
	template <typename Key, typename Value, int16_t BtreeOrder, typename NodeType>
	void
	Elements<Key, Value, BtreeOrder, NodeType>::adjustMemory(const int16_t direction, content_type *begin)
	{
		// default content_type* end
		if (_count == 0 || direction == 0) {
			return;
		} else if (direction > 0 && _count >= BtreeOrder) {
			throw runtime_error("The Elements is full");
		}
		auto&& end = _count >= BtreeOrder ? _elements.end() : &_elements[_count]; // end is exclude
		Elements::moveElement(direction, begin, end);
	}

	template <typename Key, typename Value, int16_t BtreeOrder, typename NodeType>
	int16_t
	Elements<Key, Value, BtreeOrder, NodeType>::relatedIndex(const Key &key)
	{
		// in the future, could use half search
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
	template <typename Key, typename Value, int16_t BtreeOrder, typename NodeType>
	typename Elements<Key, Value, BtreeOrder, NodeType>::content_type*
	Elements<Key, Value, BtreeOrder, NodeType>::moveElement(const int16_t direction, content_type *begin, content_type *end)
	{
#ifdef FALSE
		std::cout << "begin: " << begin << std::endl;
		std::cout << "end: " << end << std::endl;
		std::cout << "content: " << sizeof(content_type) << std::endl;
#endif

		// depend on different direction, where to start copy is different
		// in the future, could use WORD_BIT to copy
		if (direction < 0) {
			return static_cast<content_type *>(
				memcpy(begin + direction, begin, (end - begin) * sizeof(content_type)));
		} else if (direction > 0) {
			for (auto current = end - 1; current >= begin; --current) {
				return static_cast<content_type *>(
					memcpy(current + direction, current, sizeof(content_type)));
			}
		}

		return begin;
	}

	template <typename Key, typename Value, int16_t BtreeOrder, typename NodeType>
	typename Elements<Key, Value, BtreeOrder, NodeType>::content_type&
	Elements<Key, Value, BtreeOrder, NodeType>::assign(content_type& ele, NodeType* node)
	{
		ele.first = node->max_key();
		auto uni_ptr = unique_ptr<NodeType>(node);
		new (&(ele.second)) std::variant<Value, std::unique_ptr<NodeType>>(std::move(uni_ptr));
		return ele;
	}

	template <typename Key, typename Value, int16_t BtreeOrder, typename NodeType>
	typename Elements<Key, Value, BtreeOrder, NodeType>::content_type&
	Elements<Key, Value, BtreeOrder, NodeType>::assign(content_type& ele, const pair<Key, Value>& pair)
	{
		ele = pair;
		return ele;
	}

	template <typename Key, typename Value, int16_t BtreeOrder, typename NodeType>
	Value&
	Elements<Key, Value, BtreeOrder, NodeType>::value(std::variant<Value, std::unique_ptr<NodeType>>& v)
	{
		return std::get<Value>(v);
	}

	template <typename Key, typename Value, int16_t BtreeOrder, typename NodeType>
	NodeType*
	Elements<Key, Value, BtreeOrder, NodeType>::ptr(const std::variant<Value, std::unique_ptr<NodeType>>& v)
	{
		return std::get<unique_ptr<NodeType>>(v).get();
	}
}