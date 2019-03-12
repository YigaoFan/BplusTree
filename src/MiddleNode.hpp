#pragma once
#include "NodeBase.hpp"

namespace btree {
    template <typename Key, typename Value, unsigned BtreeOrder, typename BtreeType>
    class MiddleNode final : NodeBase<Key, Value, BtreeOrder, BtreeType> {
    public:
        MiddleNode();
        ~MiddleNode() override;

        void app
    };
}