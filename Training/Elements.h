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
    unsigned count_{0};
    unsigned cache_index_; // TODO: maybe Element.add will change this value
public:
    // Node-caller ensure the sort and count are right
    Elements(const std::initializer_list<std::pair<Key, void*>> li)
    {
        unsigned i = 0;
        for (std::pair<Key, void*>& p : li) {
            elements_[i] = p;
            ++i;
        }
        count_ = i;
        cache_index_ = count_ / 2;
    }
    //Elements() = default; TODO: will useful
    // Node-caller should ensure the i is right range
    Key& key(unsigned i) const
    { return elements_[i].first; }
    Key& key() const
    { return elements_[count_ - 1].first; }
    bool have(const Key&);
    void*& value(unsigned i) const
    { return elements_[i].second; }
    std::vector<Key> all_key() const
    {
        std::vector<Key> r(count_);
        for (unsigned i = 0; i < count_; ++i) {
            r[i] = elements_[i].first;
        }
        return r;
    }
    bool full() const { return count_ >= BtreeOrder; }
    template <typename Value>
    void add(const std::pair<Key, Value>&);
    template <typename Value>
    void add(const std::pair<Key, Value*>*);
};
// TODO: move to Elements
void move_Ele(const NodeIter<ele_instance_type>&, const NodeIter<ele_instance_type>&,
              unsigned=1);
