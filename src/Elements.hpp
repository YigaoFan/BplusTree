#pragma once

namespace btree {
    template <typename Key, typename Value, unsigned BtreeOrder, typename NodeType>
    class Elements {
    public:
        using content_type = std::pair<Key, std::variant<Value, std::unique_ptr<NodeType>>>;

        template <typename Iterator>
        Elements(Iterator, Iterator);

        Elements(const Elements&);
        // Common
        Key max_key() const;
        bool have(const Key&);
        std::vector<Key> all_key() const;
        bool full() const;
        bool remove(const Key&);
        template <typename T> // use T to not limit to key-value
        bool add(const std::pair<Key, T>&);
        template <typename T>
        bool append(const std::pair<Key, T>&);

        // all here bool means change

        // for Value
        std::variant<Value, std::unique_ptr<NodeType>>& operator[](const Key&);

        bool exchange_max_out(const std::pair<Key, Value>&, std::pair<Key, Value>&); // when full call this function
        // for ptr
        NodeType* ptr_of_min() const; // for Node for Btree traverse all leaf
        NodeType* ptr_of_max() const; // for add the key beyond the max bound
        bool change_key(const Key&, const Key&);

        static Value value(const std::variant<Value, std::unique_ptr<NodeType>>&);
        static NodeType* ptr(const std::variant<Value, std::unique_ptr<NodeType>>&);
    private:
        Elements() = default; // for construct null middle_type Node
        // Field
        char count_{0};
        char cache_index_{0};
        Key cache_key_; // TODO check all the place related or should exist
        std::array<content_type, BtreeOrder> elements_;

        void reset_cache();


        static content_type& move_element(const char, content_type*, content_type*);
        content_type& to_end_move(const char, content_type*);
        content_type& related_position(const Key&);

        static content_type& assign(content_type&, std::pair<Key, NodeType*>&);
        static content_type& assign(content_type&, const std::pair<Key, Value>&);
    };
}

// implementation
namespace btree {
    //#include <cstring>
    using std::pair;
    using std::vector;
    using std::unique_ptr;
    using std::memcpy;

    // public method part:

    // Common:
    // some methods are for middle, some for leaf

    template <typename Key, typename Value, unsigned BtreeOrder, typename NodeType>
    template <typename Iter>
    Elements<Key, Value, BtreeOrder, NodeType>::Elements(Iter begin, Iter end)
    {
        if (begin == end) {
            return;
        } else {
            do {
                Elements::assign(elements_[count_], *begin);

                ++count_;
                ++begin;
            } while (begin != end);
        }
    }

    template <typename Key, typename Value, unsigned BtreeOrder, typename NodeType>
    Elements<Key, Value, BtreeOrder, NodeType>::Elements(const Elements& that)
    : count_(that.count_), elements_(that.elements_)
    {
        // complete copy
    }

    template <typename Key, typename Value, unsigned BtreeOrder, typename NodeType>
    Key
    Elements<Key, Value, BtreeOrder, NodeType>::max_key() const
    {
        return elements_[count_ - 1].first;
    }


    template <typename Key, typename Value, unsigned BtreeOrder, typename NodeType>
    bool
    Elements<Key, Value, BtreeOrder, NodeType>::have(const Key& key)
    {
        // version 1:
        //    auto& cache = elements_[cache_index_];
        //    if (key == cache.first) // TODO: need define == method?
        //    {
        //        return true; // how to return false?
        //    }
        //    if (com(key, cache_index_)) {// TODO: compare function
        //        cache_index_ /= 2; // possible lose some index?
        //        this->have(key);
        //    } else {
        //        cache_index_ = (count_ - cache_index_) / 2 + cache_index_;
        //        this->have(key);
        //    }

        // version 2:
        cache_key_ = key;
        for (auto i = count_ - 1; i >= 0; --i) {
            if (elements_[i].first == key) {
                cache_index_ = i;
                return true;
            } else if (key > elements_[i].first) {
                cache_index_ = i;
                return false;
            }
        }
        // TODO here could arrange a static_assert
        //return false;
    }


    template <typename Key, typename Value, unsigned BtreeOrder, typename NodeType>
    vector<Key>
    Elements<Key, Value, BtreeOrder, NodeType>::all_key() const {
        vector<Key> r(count_);
        for (size_t i = 0; i < count_; ++i) {
            r[i] = elements_[i].first;
        }
        return r;
    }


    template <typename Key, typename Value, unsigned BtreeOrder, typename NodeType>
    bool
    Elements<Key, Value, BtreeOrder, NodeType>::Elements::full() const
    {
        return count_ >= BtreeOrder;
    }


    template <typename Key, typename Value, unsigned BtreeOrder, typename NodeType>
    bool
    Elements<Key, Value, BtreeOrder, NodeType>::remove(const Key& key)
    {
        auto& e = this->related_position(key);
        auto bound_changed(false);
        if (&e == &elements_[count_ - 1]) {
            bound_changed = true;
        }
        this->to_end_move(-1, &e + 1);
        return bound_changed;
    }

    // for Value

    template <typename Key, typename Value, unsigned BtreeOrder, typename NodeType>
    std::variant<Value, unique_ptr<NodeType>>&
    Elements<Key, Value, BtreeOrder, NodeType>::operator[](const Key& key)
    {
        // TODO have problem
        // 1. should return ptr, check_out_digging need
//        auto&& cache_k = elements_[cache_index_].first;
//        auto&& cache_v = Elements::value(elements_[cache_index_].second);
//
//        if (key == cache_k) {
//            this->reset_cache();
//            return cache_v;
//        } else if (key < cache_k) {
//            for (auto i = 0; i < cache_index_; ++i) {
//                if (key == elements_[i].first) {
//                    return Elements::value(elements_[i].second);
//                }
//            }
//        } else {
//            for (int i = cache_k; i < count_; ++i) {
//                if (key == elements_[i].first) {
//                    return Elements::value(elements_[i].second);
//                }
//            }
//        }
        // lite version:
        for (auto& e : elements_) {
            if (key == e.first) {
                return e.second;
            }
        }
    }

    // Node think first of full situation
    // return value indicate the the max-key is changed or not

    template <typename Key, typename Value, unsigned BtreeOrder, typename NodeType>
    template <typename T>
    bool
    Elements<Key, Value, BtreeOrder, NodeType>::add(const pair<Key, T>& pair)
    {
        // TODO change to return bool
        // maybe only leaf add logic use this Elements method
        // middle add logic is in middle Node self
        auto& k = pair.first;
        auto& v = pair.second;

        if (k == cache_key_) {
            auto& des = this->to_end_move(1, &elements_[cache_index_ + 1]);
            des = pair;
        } else if (k > cache_key_) {
            // TODO maybe cache_key_ and cache_index_ is not correspond
            for (auto i = cache_index_ + 1; i < count_; ++i) {
                if (k == elements_[i].first) {
                    // TODO think of dichotomy to get the index
                    auto& des = this->to_end_move(elements_[i], 1);
                    des = pair;
                }
            }
        } else {
            for (auto i = 0; i < cache_index_; ++i) {
                if (k == elements_[i].first) {
                    auto& des = this->to_end_move(elements_[i], 1);
                    des = pair;
                }
            }
        }
        ++count_;
    }


    template <typename Key, typename Value, unsigned BtreeOrder, typename NodeType>
    template <typename T>
    bool
    Elements<Key, Value, BtreeOrder, NodeType>::append(const pair<Key, T>& pair)
    {
        elements_[count_] = pair;
        ++count_;
        return true;
    }


    template <typename Key, typename Value, unsigned BtreeOrder, typename NodeType>
    bool
    Elements<Key, Value, BtreeOrder, NodeType>::exchange_max_out(const pair<Key, Value>& p, pair<Key, Value>& out_pair)
    {
        pair<Key, Value> max{ elements_[count_ - 1].first, Elements::value(elements_[count_ - 1].second) };
        for (auto& e : elements_) {
            if (p.first <= e.first) {
                auto pos = Elements::move_element(1, &e, &elements_[count_ - 2]);
                pos = p;
                out_pair = max;
                // TODO return bool
            }
        }
        // TODO assert not arrive here
    }

    // for ptr

    template <typename Key, typename Value, unsigned BtreeOrder, typename NodeType>
    NodeType*
    Elements<Key, Value, BtreeOrder, NodeType>::ptr_of_min() const
    {
        return Elements::ptr(elements_[0].second);
    }


    template <typename Key, typename Value, unsigned BtreeOrder, typename NodeType>
    NodeType*
    Elements<Key, Value, BtreeOrder, NodeType>::ptr_of_max() const
    {
        return Elements::ptr(elements_[count_ - 1].second);
    }


    template <typename Key, typename Value, unsigned BtreeOrder, typename NodeType>
    bool
    Elements<Key, Value, BtreeOrder, NodeType>::change_key(const Key& old_key, const Key& new_key)
    {
    }

    //
    //template <typename T>
    //void
    //Elements<Key, Value, BtreeOrder, NodeType>::add(const pair<Key, T*>& pair)
    //{
    //    elements_[count_].first = pair.first;
    //    elements_[count_].second.reset(pair.second);
    //    ++count_;
    //}
    // private method part:


    template <typename Key, typename Value, unsigned BtreeOrder, typename NodeType>
    Value
    Elements<Key, Value, BtreeOrder, NodeType>::value(const std::variant<Value, std::unique_ptr<NodeType>>& v)
    {
        return std::get<Value>(v);
    }


    template <typename Key, typename Value, unsigned BtreeOrder, typename NodeType>
    NodeType*
    Elements<Key, Value, BtreeOrder, NodeType>::ptr(const std::variant<Value, std::unique_ptr<NodeType>>& v)
    {
        return std::get<unique_ptr<NodeType>>(v).get();
    }


    template <typename Key, typename Value, unsigned BtreeOrder, typename NodeType>
    typename Elements<Key, Value, BtreeOrder, NodeType>::content_type&
    Elements<Key, Value, BtreeOrder, NodeType>::move_element(const char direction, content_type* begin, content_type* end)
    {
        memcpy(begin + direction, begin, end - begin);
        return *begin;
    }


    template <typename Key, typename Value, unsigned BtreeOrder, typename NodeType>
    typename Elements<Key, Value, BtreeOrder, NodeType>::content_type&
    Elements<Key, Value, BtreeOrder, NodeType>::to_end_move(const char direction, content_type* begin)
    {
        // default content_type* end
        auto end = &elements_[count_ - 1];
        return this->move_element(direction, begin, end);
    }

    // caller should ensure the key must exist

    template <typename Key, typename Value, unsigned BtreeOrder, typename NodeType>
    typename Elements<Key, Value, BtreeOrder, NodeType>::content_type&
    Elements<Key, Value, BtreeOrder, NodeType>::related_position(const Key& key)
    {
        // in the future, could use half search
        for (auto& e : elements_) {
            if (e.first == key) {
                return e;
            }
        }
    }


    template <typename Key, typename Value, unsigned BtreeOrder, typename NodeType>
    typename Elements<Key, Value, BtreeOrder, NodeType>::content_type&
    Elements<Key, Value, BtreeOrder, NodeType>::assign(content_type& ele, pair<Key, NodeType*>& pair)
    {
        ele.first = pair.first;
        auto uni_ptr = unique_ptr<NodeType>(pair.second);
        new (&(ele.second)) std::variant<Value, std::unique_ptr<NodeType>>(std::move(uni_ptr));
//        ele.second = std::move(pair.second);
        return ele;
    }


    template <typename Key, typename Value, unsigned BtreeOrder, typename NodeType>
    typename Elements<Key, Value, BtreeOrder, NodeType>::content_type&
    Elements<Key, Value, BtreeOrder, NodeType>::assign(content_type& ele, const pair<Key, Value>& pair)
    {
        ele = pair;
        return ele;
    }



    template <typename Key, typename Value, unsigned BtreeOrder, typename NodeType>
    void
    Elements<Key, Value, BtreeOrder, NodeType>::reset_cache()
    {
        cache_index_ = static_cast<char>(count_ / 2);
    }
}
