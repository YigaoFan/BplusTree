#include "Elements.h"
template <typename Key, unsigned BtreeOrder>
bool
Elements<Key, BtreeOrder>::have(const Key& key)
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