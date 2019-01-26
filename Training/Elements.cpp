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
NODE_INSTANCE::Elements::Elements(Node* belong_node, Iterator begin, Iterator end)
    : belong_node_(belong_node), count_(0)
{
    do {
        elements_[count_] = *begin;

        ++count_;
        ++begin;
    } while (begin != end);
    this->reset_cache();
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
NODE_TEMPLATE
template <typename T>
void
NODE_INSTANCE::Elements::append(const pair<Key, T>& pair)
{
    // TODO
    // maybe only leaf add logic use this Elements method
    // middle add logic is in middle Node self
    auto& k = pair.first;
    auto& v = pair.second;
    if (this->full()) {
        // exchange the max pair out, then insert this pair to next Node
    } else {
        if (k == cache_key_) {
            auto& des = Elements::move_element(&elements_[cache_index_ + 1], 1);
            des = pair;
        } else if (k > cache_key_) {
            // TODO maybe cache_key_ and cache_index_ is not correspond
            for (auto i = cache_index_ + 1; i < count_; ++i) {
                if (k == elements_[i].first)
                {
                    // TODO think of dichotomy to get the index
                    auto& des = Elements::move_element(elements_[i], 1);
                    des = pair;
                }
            }
        } else {
            for (auto i = 0; i < cache_index_; ++i) {
                if (k == elements_[i].first) {
                    auto& des = Elements::move_element(elements_[i], 1);
                    des = pair;
                }
            }
        }
    }
    // when is full, need to call wide Node method
    //elements_[count_] = pair;/*.first;
    //elements_[count_].second = pair.second;*/
    //++count_;
}

// for ptr
NODE_TEMPLATE
NODE_INSTANCE*
NODE_INSTANCE::Elements::ptr_of_min() const
{
    return Elements::ptr(elements_[0].second);
}


//NODE_TEMPLATE
//template <typename T>
//void
//NODE_INSTANCE::Elements::append(const pair<Key, T*>& pair)
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
NODE_INSTANCE::Elements::move_element(content_type* begin, const char direction)
{
    // return the begin reference
    // default content_type* end
    auto end = &elements_[count_ - 1];
    memcpy(begin + direction, begin, end - begin);
    return *begin;
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
// middle = false, elements.append(pair<Key, Value>)
// middle = true, elements.append(Node*)


