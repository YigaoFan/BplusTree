#include "Elements.h"
using namespace btree;
using std::initializer_list;
using std::pair;
using std::vector;

#define NODE_TEMPLATE template <typename Key, typename Value, unsigned BtreeOrder, typename BtreeType>
#define NODE_INSTANCE Node<Key, Value, BtreeOrder, BtreeType>

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

NODE_TEMPLATE
NODE_INSTANCE::Elements::Elements()
{
    // null
}

NODE_TEMPLATE
Key
NODE_INSTANCE::Elements::max_key() const
{
    return elements_[count_ - 1].first;
}

NODE_TEMPLATE
bool
NODE_INSTANCE::Elements::have(const Key& key) const
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

NODE_TEMPLATE
NODE_INSTANCE*
NODE_INSTANCE::Elements::min_value() const
{
    return elements_[0].second;
}

NODE_TEMPLATE
vector<Key>
NODE_INSTANCE::Elements::all_key() const {
    vector<Key> r(count_);
    for (unsigned i = 0; i < count_; ++i) {
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
template <typename T>
void
NODE_INSTANCE::Elements::add(const pair<Key, T>& pair)
{
    // Node think first of full situation
    Data* v_p = mem_alloc_->leaf_data_allocate(pair);
    elements_[count_].first = pair.first;
    elements_[count_].second.reset(v_p);
}
NODE_TEMPLATE
template <typename T>
void
NODE_INSTANCE::Elements::add(const pair<Key, T*>& pair)
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

