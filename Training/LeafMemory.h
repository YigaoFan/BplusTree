#pragma once
#include <utility> // for pair
#include <memory>
#include <vector>
#include "CommonFlag.h"
#include "RealData.h"
using namespace btree;

namespace btree {
    // LeafMemory will depend on the RealData, especially the leaf type not for middle type
    // TODO: Later I should provide a switch to turn on/off this feature in Btree constructor or template arg,
    // TODO: because this feature is willing to help to construct the Hash-Map
    template<typename Key, typename Value, unsigned Growth>
    class LeafMemory {
        using data_type = RealData<Key, Value>;
    public:
        // Factory
        static LeafMemory* produce_leaf_memory(const size_t&);

        data_type* leaf_data_allocate(const std::pair<Key, Value>&);

    private:
        std::vector<std::unique_ptr<std::vector<data_type>>> leaf_memo_{1/*count*/};
        std::vector<bool> used_mark_;
        const float growth_rate_;
        explicit LeafMemory(const size_t&);
        ~LeafMemory();
        void new_memo();
        size_t leaf_num();

        static size_t num_of_new(const bool, const size_t&, const float&);
    };
}
