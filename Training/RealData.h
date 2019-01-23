#pragma once
#include <utility>
#include "Data.h"

namespace btree {
    // For LeafMemory Heap using only temporarily
    template <typename Key, typename Value>
    class RealData : public Data {
        // using the class to use the unique_ptr 
        //  to use the type to collect the memory accurately
        std::pair<Key, Value> pair_;
        //explicit RealData(const Value&); // LeafMemory do this
    public:
        void* value() const override;
        void change_value(void*) override;
        ~RealData() override = default;

        // code below need to attention
        RealData& operator=(const std::pair<Key, Value>&);
    };
    // When RealData stores key-value, Value is value
    // When RealData stores key-pointer, Value is unique_ptr<Node>
}

