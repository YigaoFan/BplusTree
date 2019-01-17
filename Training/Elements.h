#pragma once
#include <utility> // for pair
#include <array> // for array
#include <vector> // for vector
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
    bool have(const Key&) {} // todo: try add a memory mechanism
    void*& value(unsigned i) const
    { return elements_[i].second; }
    // TODO: and other write operation
    std::vector<Key> all_key(const unsigned& key_num) const
    {
        std::vector<Key> r(key_num);
        for (unsigned i = 0; i < key_num; ++i) {
            r[i] = elements_[i].first;
        }
        return r;
    }
};
// TODO: move to Elements
void move_Ele(const NodeIter<ele_instance_type>&, const NodeIter<ele_instance_type>&,
              unsigned=1);
