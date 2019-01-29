#include <cstring>
#include "Elements.h"
using namespace btree;
using std::initializer_list;
using std::pair;
using std::vector;
using std::unique_ptr;
using std::memcpy;
#define NODE_TEMPLATE template <typename Key, typename Value, unsigned BtreeOrder, typename BtreeType>
#define NODE_INSTANCE Node<Key, Value, BtreeOrder, BtreeType>

// public method part:

// Common:
// some methods are for middle, some for leaf

//NODE_TEMPLATE
//template <typename T>
//NODE_INSTANCE::Elements::Elements(const initializer_list<pair<Key, T>> li)
//    :count_(li.size()), cache_index_(this->reset_cache()), elements_{li}
//{
//    // null
//}
//
//NODE_TEMPLATE
//template <typename T>
//NODE_INSTANCE::Elements::Elements(const initializer_list<pair<Key, T*>> li)
//    :count_(li.size()), cache_index_(this->reset_cache())
//{
//    auto i = 0;
//    for (auto& e : li) {
//        elements_[i].first = e.first;
//        elements_[i].second = unique_ptr<Node>(e.second);
//        ++i;
//    }
//}

NODE_TEMPLATE
template <typename Iterator>
NODE_INSTANCE::Elements::Elements(Iterator begin, Iterator end)
    : count_(0)
{
    if (begin == end) {

    } else {
        do {
            elements_[count_] = *begin;

            ++count_;
            ++begin;
        } while (begin != end);
        this->reset_cache();
    }
}

NODE_TEMPLATE
Key
NODE_INSTANCE::Elements::max_key() const
{
    return elements_[count_ - 1].first;
}

NODE_TEMPLATE
bool
NODE_INSTANCE::Elements::have(const Key& key)
{
    // version 1:
//    auto& cache = elements_[cache_index_];
//    if (key == cache.first) // TODO: need define == method?
//    {
//        return true; // how to return false?
//    }
//    if (com(key, cache_index_)) {// TODO: compare function
//        cache_index_ /= 2; // possible lose some index?
//        this->have(key);
//    } else {
//        cache_index_ = (count_ - cache_index_) / 2 + cache_index_;
//        this->have(key);
//    }

    // version 2:
    cache_key_ = key;
    for (auto i = count_ - 1; i >= 0; --i) {
        if (elements_[i].first == key) {
            cache_index_ = i;
            return true;
        } else if (key > elements_[i].first) {
            cache_index_ = i;
            return false;
        }
    }
    // TODO here could arrange a static_assert
    //return false;
}

NODE_TEMPLATE
vector<Key>
NODE_INSTANCE::Elements::all_key() const {
    vector<Key> r(count_);
    for (size_t i = 0; i < count_; ++i) {
        r[i] = elements_[i].first;
    }
    return r;
}

NODE_TEMPLATE
bool
NODE_INSTANCE::Elements::full() const
{
    return count_ >= BtreeOrder;
}

NODE_TEMPLATE
void 
NODE_INSTANCE::Elements::remove(const Key&)
{
    // TODO
    // may need to distinguish middle or not
    // if change the max_value or balance, call the BtreeHelper
}

// for Value
NODE_TEMPLATE
Value&
NODE_INSTANCE::Elements::operator[](const Key& key)
{
    // TODO 
    // 1. should return ptr, check_out_digging need
    auto& cache_k = elements_[cache_index_].first;
    auto& cache_v = Elements::value(elements_[cache_index_].second);

    if (key == cache_k) {
        this->reset_cache();
        return cache_v;
    } else if (key < cache_k) {
        for (auto i = 0; i < cache_index_; ++i) {
            if (key == elements_[i].first) {
                return Elements::value(elements_[i].second);
            }
        }
    } else {
        for (int i = cache_k; i < count_; ++i) {
            if (key == elements_[i].first) {
                return Elements::value(elements_[i].second);
            }
        }
    }
}

// Node think first of full situation
// return value indicate the the max-key is changed or not
NODE_TEMPLATE
template <typename T>
bool
NODE_INSTANCE::Elements::add(const pair<Key, T>& pair)
{
    // TODO change to return bool
    // maybe only leaf add logic use this Elements method
    // middle add logic is in middle Node self
    auto& k = pair.first;
    auto& v = pair.second;

    if (k == cache_key_) {
        auto& des = this->up_to_end_move(1, &elements_[cache_index_ + 1]);
        des = pair;
    } else if (k > cache_key_) {
        // TODO maybe cache_key_ and cache_index_ is not correspond
        for (auto i = cache_index_ + 1; i < count_; ++i) {
            if (k == elements_[i].first) {
                // TODO think of dichotomy to get the index
                auto& des = this->up_to_end_move(elements_[i], 1);
                des = pair;
            }
        }
    } else {
        for (auto i = 0; i < cache_index_; ++i) {
            if (k == elements_[i].first) {
                auto& des = this->up_to_end_move(elements_[i], 1);
                des = pair;
            }
        }
    }
    ++count_;
}

NODE_TEMPLATE
template <typename T>
bool
NODE_INSTANCE::Elements::append(const pair<Key, T>& pair)
{
    elements_[count_] = pair;
    ++count_;
    return true;
}

NODE_TEMPLATE
pair<Key, Value> 
NODE_INSTANCE::Elements::exchange_max_out(const pair<Key, Value>& p)
{
    pair<Key, Value> max{elements_[count_ - 1].first, Elements::value(elements_[count_ - 1].second)};
    for (auto& e : elements_) {
        if (p.first <= e.first) {
            auto pos = Elements::move_element(1, &e, &elements_[count_ - 2]);
            pos = p;
            return max;
        }
    }
    // TODO assert not arrive here
}

// for ptr
NODE_TEMPLATE
NODE_INSTANCE*
NODE_INSTANCE::Elements::ptr_of_min() const
{
    return Elements::ptr(elements_[0].second);
}

NODE_TEMPLATE
NODE_INSTANCE*
NODE_INSTANCE::Elements::ptr_of_max() const
{
    return Elements::ptr(elements_[count_ - 1].second);
}


//NODE_TEMPLATE
//template <typename T>
//void
//NODE_INSTANCE::Elements::add(const pair<Key, T*>& pair)
//{
//    elements_[count_].first = pair.first;
//    elements_[count_].second.reset(pair.second);
//    ++count_;
//}
// private method part:

NODE_TEMPLATE
Value 
NODE_INSTANCE::Elements::value(const std::variant<Value, std::unique_ptr<Node>>& v)
{
    return std::get<Value>(v);
}

NODE_TEMPLATE
NODE_INSTANCE*
NODE_INSTANCE::Elements::ptr(const std::variant<Value, std::unique_ptr<Node>>& v)
{
    return std::get<unique_ptr<Node>>(v).get();
}

NODE_TEMPLATE
typename NODE_INSTANCE::Elements::content_type& 
NODE_INSTANCE::Elements::move_element(const char direction, content_type* begin, content_type* end)
{
    memcpy(begin + direction, begin, end - begin);
    return *begin;
}

NODE_TEMPLATE
typename NODE_INSTANCE::Elements::content_type& 
NODE_INSTANCE::Elements::up_to_end_move(const char direction, content_type* begin)
{
    // default content_type* end
    auto end = &elements_[count_ - 1];
    return this->move_element(direction, begin, end);
}


NODE_TEMPLATE
void
NODE_INSTANCE::Elements::reset_cache()
{
    cache_index_ = count_ / 2;
}

// Btree construct lots of Node instance, need to construct upper Node
// Elements through the Node constructor get these Node* and Value
// 1, Value must be saved a copy, Node* just need to save it
// 2, We need to make two of them are fit in Elements
// 3, how to do?
// middle = false, elements.add(pair<Key, Value>)
// middle = true, elements.add(Node*)


