// todo: this file is plan to be a header or cpp file?
// todo: it will make difference. like "using".
#include "Btree.h"

namespace btree {
    template<typename Key, typename Value, typename Compare, unsigned BtreeOrder>
    Btree<Key, Value, Compare, BtreeOrder>
        construct_a_btree(Compare& comp)
    {
        static Compare compare = comp;
        // Btree definition // todo: use macro to implement?

        // Node definition // todo: use macro to implement?

        return; // a btree object
    }
}


