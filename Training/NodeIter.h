#ifndef NODE_ITER_H
#define NODE_ITER_H

#include <iterator>
//#include <tclDecls.h>

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
// todo: Or I must use special Itertor?
template <typename Item>
class NodeIter
        : public std::iterator<std::forward_iterator_tag, Item> {
    Item* ptr;

    // not very clear to add "explicit" below, because of single-arg
    explicit NodeIter(Item* p = nullptr)
    : ptr(p) {}

    Item& operator*() const { return *ptr; }
    Item* operator->() const { return ptr; }

    // todo: implement the ptr->next
    NodeIter& operator++() { ptr = ptr->next(); return *this; }

    bool operator==(const NodeIter& i) const
    { return ptr == i.ptr; }
    bool operator!=(const NodeIter& i) const
    { return ptr != i.ptr; }

};

//template <typename I>
//struct A {
//    typedef typename I::value_type value_type;
//};
#endif
