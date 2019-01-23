#include "LeafMemory.h"
#include <cmath>
using namespace btree;
using std::pair;
using std::vector;
using std::ceil;
using std::unique_ptr;
#define LEAFMEMORY_TEMPLATE template <typename Key, typename Value, unsigned Growth>
#define LEAFMEMORY_INSTANCE LeafMemory<Key, Value, Growth>

LEAFMEMORY_TEMPLATE
LEAFMEMORY_INSTANCE*
LEAFMEMORY_INSTANCE::produce_leaf_memory(const size_t& size)
{
    return new LeafMemory(size);
}

LEAFMEMORY_TEMPLATE
typename LEAFMEMORY_INSTANCE::data_type*
LEAFMEMORY_INSTANCE::leaf_data_allocate(const pair<Key, Value>& pair)
{
    // get memory
    data_type* ret = this->usable_memo();
    // copy into
    return &ret->operator=(pair);   
}

LEAFMEMORY_TEMPLATE
LEAFMEMORY_INSTANCE::LeafMemory(const size_t& size)
    : used_mark_(size, false),
    growth_rate_(Growth / 10.0)
{
    leaf_memo_[0] = unique_ptr<vector<data_type>>(
        new vector<data_type>(
            LeafMemory::template num_of_new<true>(size, growth_rate_)));
}

LEAFMEMORY_TEMPLATE
LEAFMEMORY_INSTANCE::~LeafMemory() = default;

// add new memory, return the first one of item and mark it used
LEAFMEMORY_TEMPLATE
typename LEAFMEMORY_INSTANCE::data_type*
LEAFMEMORY_INSTANCE::new_memo()
{
    // template member method when prefix like this and . -> need to be used like this
    auto n = LeafMemory::template num_of_new<false>(leaf_num(), growth_rate_);
    // new memory
    auto new_memo = new vector<data_type>(n);
    leaf_memo_.push_back(unique_ptr<vector<data_type>>(new_memo));
    // add mark
    used_mark_.insert(used_mark_.end(), true);
    used_mark_.insert(used_mark_.end(), n-1, false);
    return &new_memo[0];
}

LEAFMEMORY_TEMPLATE
size_t
LEAFMEMORY_INSTANCE::leaf_num() const
{
    return used_mark_.size();
}

LEAFMEMORY_TEMPLATE
typename LEAFMEMORY_INSTANCE::data_type*
LEAFMEMORY_INSTANCE::usable_memo()
{
    for (auto i = 0; i < used_mark_.size(); ++i) {
        if (used_mark_[i] == false) {
            used_mark_[i] = true;
            return this->correspond_memo(i);
        }
    }

    return this->new_memo();
}

LEAFMEMORY_TEMPLATE
typename LEAFMEMORY_INSTANCE::data_type* 
LEAFMEMORY_INSTANCE::correspond_memo(const size_t& index) const
{
    size_t left_border = 0;
    size_t right_border;
    for (auto& ptr : leaf_memo_) {
        right_border = left_border + ptr->size();
        if (index <= right_border) {
            return ptr->operator[](index - left_border);
        } else {
            left_border += ptr->size();
        }
    }
}

LEAFMEMORY_TEMPLATE
template <bool First>
size_t
LEAFMEMORY_INSTANCE::num_of_new(const size_t& base, const float& growth_rate)
{
    if constexpr (First) {
        return static_cast<size_t>(ceil(base * (1 + growth_rate)));
    } else {
        return static_cast<size_t>(ceil(base * growth_rate));
    }
}


