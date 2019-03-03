#pragma once
//template <typename >
class BtreeHelper {
public:
    BtreeHelper() = default;
    virtual ~BtreeHelper() = default;
    // the BtreeHelper has need to be Class?
    template <typename NodeType, typename Key>
    void change_bound_upwards(NodeType*, const Key&, const Key&) const;
    // when a lot of place use this template argument, we should add it to class template
};
