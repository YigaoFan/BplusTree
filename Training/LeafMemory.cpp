#include "LeafMemory.h"
using namespace btree;
using std::pair;
#define LEAFMEMORY_TEMPLATE template <typename Key, typename Value>
#define LEAFMEMORY_INSTANCE LeafMemory<Key, Value>

LEAFMEMORY_TEMPLATE
LEAFMEMORY_INSTANCE*
LEAFMEMORY_INSTANCE::produce_leaf_memory(const size_t& size)
{
    return new LeafMemory(size);
}

LEAFMEMORY_TEMPLATE
RealData<Key, Value>*
LEAFMEMORY_INSTANCE::leaf_data_allocate(const pair<Key, Value>&)
{

}

LEAFMEMORY_TEMPLATE
LEAFMEMORY_INSTANCE::LeafMemory(const size_t& size)
{
    // properly size=0
}

LEAFMEMORY_TEMPLATE
LEAFMEMORY_INSTANCE::~LeafMemory()
{
}
