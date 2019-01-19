#pragma once
#include <utility> // for pair
#include <array> // for array
#include <vector> // for vector
#include <initializer_list>

namespace btree {
    /// This will require Key has default constructor
template <typename Key, unsigned BtreeOrder>
class Elements {
private:
    std::array<std::pair<Key, void*>, BtreeOrder> elements_;
    unsigned count_{0};
    unsigned cache_index_{0}; // TODO: maybe Element.add will change this min_value
public:
    // Node-caller ensure the sort and count are right
//    Elements(const std::initializer_list<std::pair<Key, void*>>); // TODO: wait for usage
    Elements() = default;
    ~Elements() = default;
    Key& max_key() const; // for Node get max_key
    bool have(const Key&);
    void*& min_value() const; // for Node get the smallest key's value, can be changed to min_value() to simplify
    std::vector<Key> all_key() const;
    inline bool full() const;
    template <typename Value>
    void add(const std::pair<Key, Value>&);
    template <typename Value>
    void add(const std::pair<Key, Value*>&);
    void*& operator[](const Key&);

private:
    inline void reset();
};
// TODO: move to Elements
//void move_Ele(const NodeIter<ele_instance_type>&, const NodeIter<ele_instance_type>&,
//              unsigned=1);
}

