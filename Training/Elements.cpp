#include "Elements.h"
#define ELEMENT_TEMPLATE_DECLARE template <typename Key, unsigned BtreeOrder>
#define ELEMENT_INSTANCE Elements<Key, BtreeOrder>
ELEMENT_TEMPLATE_DECLARE
bool
ELEMENT_INSTANCE::have(const Key& key)
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
        } else {
            return false;
        }
    }
}

ELEMENT_TEMPLATE_DECLARE
void*&
ELEMENT_INSTANCE::operator[](const Key& key)
{
    auto& cache_k = elements_[cache_index_].first;
    auto& cache_v = elements_[cache_index_].second;

    if (key == cache_k) {
        return cache_v;
    } else if (key < cache_k) {

    }
}
