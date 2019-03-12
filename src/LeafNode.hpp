#pragma once
#include "NodeBase.hpp"

namespace btree {
    template <typename Key, typename Value, unsigned BtreeOrder, typename BtreeType>
    class LeafNode final : NodeBase<Key, Value, BtreeOrder, BtreeType> {
        LeafNode();

        ~LeafNode() override;
    };
}