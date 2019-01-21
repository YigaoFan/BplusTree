#pragma once
#include <utility>
#include "CommonFlag.h"
#include "RealData.h"
using namespace btree;

namespace btree {
    // LeafMemory will depend on the RealData, especially the leaf type not for middle type
    // TODO: Later I should provide a switch to turn on/off this feature in Btree constructor,
    // TODO: because this feature is willing to help to construct the Hash-Map
    template<typename Key, typename Value>
    class LeafMemory {
    public:
        // Factory
        static LeafMemory* produce_leaf_memory(const size_t&);

        RealData<Key, Value>* leaf_data_allocate(const std::pair<Key, Value>&);

    private:

        explicit LeafMemory(const size_t&);
        ~LeafMemory();
    };
}
