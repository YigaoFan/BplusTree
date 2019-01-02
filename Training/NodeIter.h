#ifndef NODE_ITER_H
#define NODE_ITER_H

#include <iterator>

// todo: Need I define the struct below?
// todo: I am not very clear the code like the below is in library or user-code
//template <typename I>
//struct iterator_traits {
//    using iterator_category = typename I::iterator_category;
//    using value_type = typename I::value_type;
//    using difference_type = typename I::difference_type;
//    using pointer = typename I::pointer;
//    using reference = typename I::reference;
//};

// point to Node::Ele
// todo: Or I must use special Iterator?
template <typename Item>
class NodeIter
        : public std::iterator<std::forward_iterator_tag, Item> {
    Item* ptr_;

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

};

//template <typename I>
//struct A {
//    typedef typename I::value_type value_type;
//};
#endif
