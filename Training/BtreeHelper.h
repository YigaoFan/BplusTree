#pragma once
//template <typename >
class BtreeHelper {
public:
    BtreeHelper() = default;
    virtual ~BtreeHelper() = default;
    template <typename NodeType, typename Key> 
    // when a lot of place use this template argument, we should add it to class template
    void change_bound_upwards(NodeType*, const Key&, const Key&) const;
};
// where suitable place to call this Helper
