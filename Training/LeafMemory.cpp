#include "LeafMemory.h"
#include <cmath>
using namespace btree;
using std::pair;
using std::vector;
using std::ceil;
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
    // TODO
    // a useable memory back
    // copy pair into it
    for (auto i = 0; i < leaf_memo_->size(); ++i) {
        if (used_mark_[i] == false) {
            leaf_memo_->operator[](i) = pair;
            used_mark_[i] = true;
            return &leaf_memo_->operator[](i);
        }
    }

    // call for new memory
}

LEAFMEMORY_TEMPLATE
LEAFMEMORY_INSTANCE::LeafMemory(const size_t& size)
    : used_mark_(size, false),
    growth_rate_(Growth / 10)
{
    leaf_memo_[0] = new vector<data_type>(LeafMemory::num_of_new(true, size, growth_rate_));
}

LEAFMEMORY_TEMPLATE
LEAFMEMORY_INSTANCE::~LeafMemory() = default;

LEAFMEMORY_TEMPLATE
void
LEAFMEMORY_INSTANCE::new_memo()
{
    leaf_memo_.push_back(new vector<data_type>(LeafMemory::num_of_new(false, leaf_num(), growth_rate_)));
}

LEAFMEMORY_TEMPLATE
size_t
LEAFMEMORY_INSTANCE::leaf_num()
{
    size_t res = 0;
    for (auto& v_ptr : leaf_memo_) {
        res += v_ptr->size();
    }
    return res;
}

LEAFMEMORY_TEMPLATE
size_t
LEAFMEMORY_INSTANCE::num_of_new(const bool first, const size_t& base, const float& growth_rate)
{
    if (first) {
        return static_cast<size_t>(ceil(base * (1 + growth_rate)));
    } else {
        return static_cast<size_t>(ceil(base * growth_rate));
    }
}


