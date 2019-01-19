#include "Elements.h"
using namespace btree;
using std::initializer_list;
using std::pair;
using std::vector;

#define ELEMENTS_TEMPLATE_DECLARE template <typename Key, unsigned BtreeOrder>
#define ELEMENTS_INSTANCE Elements<Key, BtreeOrder>

// public method part:

//ELEMENTS_TEMPLATE_DECLARE
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
ELEMENTS_TEMPLATE_DECLARE
Key&
ELEMENTS_INSTANCE::max_key() const
{
    return elements_[count_ - 1].first;
}

ELEMENTS_TEMPLATE_DECLARE
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

ELEMENTS_TEMPLATE_DECLARE
void *&
ELEMENTS_INSTANCE::min_value() const
{
    return elements_[0].second;
}

ELEMENTS_TEMPLATE_DECLARE
vector<Key>
ELEMENTS_INSTANCE::all_key() const {
    vector<Key> r(count_);
    for (unsigned i = 0; i < count_; ++i) {
        r[i] = elements_[i].first;
    }
    return r;
}

ELEMENTS_TEMPLATE_DECLARE
bool
ELEMENTS_INSTANCE::full() const
{
    return count_ >= BtreeOrder;
}

ELEMENTS_TEMPLATE_DECLARE
template <typename Value>
void
ELEMENTS_INSTANCE::add(const pair<Key, Value>& pair)
{
    // todo
}

ELEMENTS_TEMPLATE_DECLARE
template <typename Value>
void
ELEMENTS_INSTANCE::add(const pair<Key, Value*>& pair)
{
    // todo
}

ELEMENTS_TEMPLATE_DECLARE
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
ELEMENTS_TEMPLATE_DECLARE
void
ELEMENTS_INSTANCE::reset()
{
    cache_index_ = count_ / 2;
}

// Btree construct lots of Node instance, need to construct upper Node
// Elements through the Node constructor get these Node* and Value
// 1, Value must be saved a copy, Node* just need to save it
// 2, We need to make two of them are fit in Elements
// 3, how to do?
