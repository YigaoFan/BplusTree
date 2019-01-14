#pragma once
#include <utility> // for pair
#include <array> // for array
#include <initializer_list>

/// This will require Key has default constructor
template <typename Key,
        unsigned BtreeOrder>
class Elements {
private:
    // Not initial, just for memory space
    std::array<std::pair<Key, void*>, BtreeOrder> elements_;
public:
    // Node-caller ensure the sort and count are right
    Elements(const std::initializer_list<std::pair<Key, void*>> li)
    {
        unsigned i = 0;
        for (std::pair<Key, void*>& p : li) {
            elements_[i] = p;
            ++i;
        }
    }
    // Node-caller should ensure the i is right range
    Key& key(unsigned i) const
    { return elements_[i].first; }
    void*& value(unsigned i) const
    { return elements_[i].second; }
    // TODO: and other write operation
};
// TODO: move to Elements
void move_Ele(const NodeIter<ele_instance_type>&, const NodeIter<ele_instance_type>&,
              unsigned=1);
