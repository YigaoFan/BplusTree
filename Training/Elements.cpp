#include "Elements.h"
using namespace btree;
using std::initializer_list;
using std::pair;
using std::vector;

#define ELEMENTS_TEMPLATE template <typename Key, unsigned BtreeOrder>
#define ELEMENTS_INSTANCE Elements<Key, BtreeOrder>

// public method part:

//ELEMENTS_TEMPLATE
//ELEMENTS_INSTANCE::Elements(const initializer_list<pair<Key, void*>> li)
//{
//    unsigned i = 0;
//    for (std::pair<Key, void*>& p : li) {
//        elements_[i] = p;
//        ++i;
//    }
//    count_ = i;
//    this->reset();
//}
ELEMENTS_TEMPLATE
Key&
ELEMENTS_INSTANCE::max_key() const
{
    return elements_[count_ - 1].first;
}

ELEMENTS_TEMPLATE
bool
ELEMENTS_INSTANCE::have(const Key& key)
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
    for (unsigned i = 0; i < count_; ++i)
    {
        if (elements_[i].first == key) {
            cache_index_ = i;
            return true;
        }
    }
    return false;
}

ELEMENTS_TEMPLATE
void *&
ELEMENTS_INSTANCE::min_value() const
{
    return elements_[0].second;
}

ELEMENTS_TEMPLATE
vector<Key>
ELEMENTS_INSTANCE::all_key() const {
    vector<Key> r(count_);
    for (unsigned i = 0; i < count_; ++i) {
        r[i] = elements_[i].first;
    }
    return r;
}

ELEMENTS_TEMPLATE
bool
ELEMENTS_INSTANCE::full() const
{
    return count_ >= BtreeOrder;
}

ELEMENTS_TEMPLATE
template <typename Value>
void
ELEMENTS_INSTANCE::add(const pair<Key, Value>& pair)
{
    // todo: malloc a manual memory
    // Node will think first of full situation
    Value* v = new Value(pair.second);
    elements_[count_];// = make_real_pair;
}

ELEMENTS_TEMPLATE
template <typename Value>
void
ELEMENTS_INSTANCE::add(const pair<Key, Value*>& pair)
{
    // todo
}

ELEMENTS_TEMPLATE
void*&
ELEMENTS_INSTANCE::operator[](const Key& key)
{
    auto& cache_k = elements_[cache_index_].first;
    auto& cache_v = elements_[cache_index_].second;

    if (key == cache_k) {
        return cache_v;
    } else if (key < cache_k) {
        for (int i = 0; i < cache_index_; ++i) {
            if (key == elements_[i].first) {
                return elements_[i].second;
            }
        }
    } else {
        for (int i = cache_k; i < count_; ++i) {
            if (key == elements_[i].first) {
                return elements_[i].second;
            }
        }
    }
}

// private method part:
ELEMENTS_TEMPLATE
void
ELEMENTS_INSTANCE::reset()
{
    cache_index_ = count_ / 2;
}

#include <memory>
using std::shared_ptr;
// Btree construct lots of Node instance, need to construct upper Node
// Elements through the Node constructor get these Node* and Value
// 1, Value must be saved a copy, Node* just need to save it
// 2, We need to make two of them are fit in Elements
// 3, how to do?
// middle = false, elements.add(pair<Key, Value>)
// middle = true, elements.add(Node*)
{
    template <typename Key, typename Value>
    void
    add(pair<Key, Value> pair)
    {
        // Change the elements.key&value
        array<pair<Key, shared_ptr<Value>>
    }

    template <typename T>
    void
    add(T* node_ptr)
    {
        // Change the elements.key&value
        auto k = node_ptr->max_key();
        array<pair<Key, shared_ptr<T>>;

    }




}

