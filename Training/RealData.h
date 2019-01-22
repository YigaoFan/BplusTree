#pragma once
#include <utility>
#include <memory>
#include "Data.h"

namespace btree {
    // For LeafMemory Heap using only temporarily
    template <typename Key, typename Value>
    class RealData : public Data {
        // using the class to use the unique_ptr 
        //  to use the type to collect the memory accurately
        std::pair<Key, Value> pair_;
        // maybe constructor is no using
        // , because the default constructing in Node
        explicit RealData(const Value&);
    public:
        // code below need to attention
        void operator=(const std::pair<Key, Value>&);

        void* value() const override;
        void change_value(void*) override;
        ~RealData() override = default;
    };
    // When RealData stores key-value, Value is value
    // When RealData stores key-pointer, Value is unique_ptr<Node>
}

