#pragma once
#include <iterator>

// point to Node::Ele
// todo: Or I must use special Iterator?
template <typename Item>
class NodeIter
        : public std::iterator<std::forward_iterator_tag, Item> {
    Item* ptr_;

public:
    // not very clear to add "explicit" below, because of single-arg
    explicit NodeIter(Item* p = nullptr)
    : ptr_(p) {}

    Item& operator*() const { return *ptr_; }
    Item* operator->() const { return ptr_; }

    NodeIter& operator++() { ptr_ = ptr_->next(); return *this; }

    bool operator==(const NodeIter& i) const
    { return ptr_ == i.ptr_; }
    bool operator!=(const NodeIter& i) const
    { return ptr_ != i.ptr_; }
    size_t operator-(NodeIter& i) const
    { return ptr_ - i.ptr_; }
    NodeIter operator-(size_t distance) const
    { return NodeIter(ptr_ - distance); }
};

//template <typename I>
//struct A {
//    typedef typename I::value_type value_type;
//};
