#pragma once

#include "Node.h"

namespace btree {
    // Result related:
    using RESULT_FLAG = int;
    constexpr int OK = 0;
    constexpr int NOT_OK = 1;
    // how to use
    /*template<typename Key, typename Value, unsigned BtreeOrder, typename BtreeType>
    using NODE_INSTANCE = class Node<Key, Value, BtreeOrder, BtreeType>;*/
}



// todo: find the file which need tod include this file.