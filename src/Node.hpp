#pragma once 

#include <utility> // for pair
#include <vector> // for vector
#include <memory> // for unique_ptr
#include <variant> // for variant

namespace btree {
    struct leaf_type final {};
    struct middle_type final {};

    // When a Node is Created, its all type is done!
	template<typename Key, typename Value, unsigned BtreeOrder, typename BtreeType>
	class Node {
		friend BtreeType; // for Btree set next_node_ TODO should reduce this access way
		class Elements;
	public:
        const bool middle;

        template <typename Iterator>
        // point to key-value array
        Node(const BtreeType *, const leaf_type, Iterator, Iterator, Node* const = nullptr);
        template <typename Iterator>
        // point to key-ptr array
        Node(const BtreeType *, const middle_type, Iterator = nullptr, Iterator = nullptr, Node* const = nullptr);
        ~Node() = default;

        bool have(const Key&);
		Value& operator[](const Key&);
        void add(const std::pair<Key, Value>&);
        void middle_append(const std::pair<Key, std::unique_ptr<Node>>&, const bool=false);
        void remove(const Key&);
        std::vector<Key> all_key() const;
        Key max_key() const;
        Node* min_leaf() const; // for Btree traverse get the leftest leaf
        Node* max_leaf() const; // for Btree get the rightest leaf
        void change_key(const Key&, const Key&);

	private:
		// Field
        Node* next_node_{nullptr};
        const BtreeType* btree_;
        Node* father_;
        Elements elements_;

        // Helper 
        bool full() const;
        void element_add(const std::pair<Key, Value>&);
        void father_add(const std::pair<Key, Value>&);

        class Elements {
        public:
            using content_type = std::pair<Key, std::variant<Value, std::unique_ptr<Node>>>;

            template <typename Iterator>
            Elements(Iterator, Iterator);
            /*template <typename T>
            Elements(const std::initializer_list<std::pair<Key, T*>>);*/
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
            Value& operator[](const Key&);

            std::pair<Key, Value> exchange_max_out(const std::pair<Key, Value>&); // when full call this function
            // for ptr
            Node* ptr_of_min() const; // for Node for Btree traverse all leaf
            Node* ptr_of_max() const; // for add the key beyond the max bound
            bool change_key(const Key&, const Key&);

        private:
            Elements() = default; // for construct null middle_type Node
            // Field
            char count_;
            char cache_index_{0};
            Key cache_key_;
            std::array<content_type, BtreeOrder> elements_{};

            void reset_cache();

            static Value value(const std::variant<Value, std::unique_ptr<Node>>&);
            static Node* ptr(const std::variant<Value, std::unique_ptr<Node>>&);
            static content_type& move_element(const char, content_type*, content_type*);
            content_type& to_end_move(const char, content_type*);
            content_type& related_position(const Key&);

            static content_type& assign(content_type&, const std::pair<Key, std::unique_ptr<Node>>&);
            static content_type& assign(content_type&, const std::pair<Key, Value>&);
        };
    };
}

#include <utility>

// implementation
namespace btree {
    using std::pair;
    using std::vector;
    using std::unique_ptr;
#define NODE_TEMPLATE template <typename Key, typename Value, unsigned BtreeOrder, typename BtreeType>
#define NODE_INSTANCE Node<Key, Value, BtreeOrder, BtreeType>

    NODE_TEMPLATE
        template <typename Iterator>
    NODE_INSTANCE::Node(const BtreeType* btree, const leaf_type, Iterator begin, Iterator end, Node* const father)
        : middle(false), btree_(btree), father_(father), elements_(begin, end)
    {
        // null
    }

    NODE_TEMPLATE
        template <typename Iterator>
    NODE_INSTANCE::Node(const BtreeType* btree, const middle_type, Iterator begin, Iterator end, Node* const father)
        : middle(true), btree_(btree), father_(father), elements_(begin, end)
    {
        // null
    }

    //NODE_TEMPLATE
    //NODE_INSTANCE::~Node() = default;

    NODE_TEMPLATE
        bool
        NODE_INSTANCE::have(const Key& k)
    {
        if (middle) {
            return false;
        } else {
            return elements_.have(k);
        }

    }

    /// Btree should use have() check or other me to ensure existing
    NODE_TEMPLATE
        Value&
        NODE_INSTANCE::operator[](const Key& k)
    {
        return elements_[k];
    }

    NODE_TEMPLATE
        void
        NODE_INSTANCE::add(const pair<Key, Value>& pair)
    {
        // TODO
        // the logic maybe not need to like this
        // because the Btree has process a lot of thing
        //if (middle) {
        //    // means here root_ add
        //    this->middle_add(pair); // need to process the return value
        //} else {
        this->element_add(pair);
        //}
    }

    // append for middle node
    NODE_TEMPLATE
        void
        NODE_INSTANCE::middle_append(const pair<Key, unique_ptr<Node>>& pair, const bool need_check_full)
    {
        if (!need_check_full || !this->full()) {
            elements_.append(pair); // for Btree add
        } else {
            auto n = new Node(this, middle_type(), &pair, &pair + 1);
            next_node_ = n;

            if (father_ == nullptr) {
                btree_->merge_branch(pair.first, n);
            } else {
                father_->middle_append({ pair.first, n });
            }
        }
    }

    // Btree has ensured the key must exist
    NODE_TEMPLATE
        void
        NODE_INSTANCE::remove(const Key& key)
    {
        auto save_key = this->max_key();
        if (elements_.remove(key)) {
            btree_->change_bound_upwards(this, save_key, this->max_key());
        }
        // TODO maybe need merge sibling
    }

    NODE_TEMPLATE
        vector<Key>
        NODE_INSTANCE::all_key() const
    {
        return elements_.all_key();
    }

    NODE_TEMPLATE
        Key
        NODE_INSTANCE::max_key() const
    {
        return elements_.max_key();
    }

    NODE_TEMPLATE
        NODE_INSTANCE*
        NODE_INSTANCE::min_leaf() const
    {
        return elements_.ptr_of_min();
    }

    NODE_TEMPLATE
        NODE_INSTANCE*
        NODE_INSTANCE::max_leaf() const
    {
        return elements_.ptr_of_max();
    }

    NODE_TEMPLATE
        void
        NODE_INSTANCE::change_key(const Key& old_key, const Key& new_key)
    {
        auto save_key = this->max_key();
        if (elements_.change_key(old_key, new_key)) {
            btree_->change_bound_upwards(this, old_key, this->max_key());
        }
    }

    // private method part:

    NODE_TEMPLATE
        bool
        NODE_INSTANCE::full() const
    {
        return elements_.full();
    }

    //NODE_TEMPLATE
    //RESULT_FLAG
    //NODE_INSTANCE::no_area_add(pair<Key, Value> pair)
    //{
    //    NodeIter<ele_instance_type> end = this->end();
    //    // todo: care here is rvalue reference and modify other place
    //    for (NodeIter<ele_instance_type>&& iter = this->begin(); iter != end; ++iter) {
    //        if (btree_->compare_func_(pair.first, iter->key())) {
    //            ele_instance_type copy = *end;
    //            this->move_Ele(iter, end - 1);
    //            iter->leaf = pair;
    //            // todo: call another way to process the temp ele_instance_type
    //        }
    //    }
    //}
    //
    //NODE_TEMPLATE
    //RESULT_FLAG
    //NODE_INSTANCE::area_add(const pair<Key, Value>& pair)
    //{
    //    NodeIter<ele_instance_type> end = this->end();
    //
    //    for (NodeIter<ele_instance_type> iter = this->begin(); iter != end; ++iter) {
    //        // once the pair.key < e.key, arrive the insert position
    //        if (btree_->compare_func_(pair.first, iter->key())) {
    //            this->move_Ele(iter, this->end());
    //            iter->key() = pair.first;
    //            iter->__value() = pair.second;
    //            ++(this->elements_count_);
    //            return OK;
    //        }
    //    }
    //}

    NODE_TEMPLATE
        void
        NODE_INSTANCE::element_add(const std::pair<Key, Value>&  pair)
    {
        if (elements_.full()) {
            // add
            auto p = elements_.exchange_max_out(pair);
            // TODO not very clear to the adjust first, or process other related Node first
            auto save_key = this->max_key();
            btree_->change_bound_upwards(this, save_key, this->max_key());
            // next node add
            if (next_node_ != nullptr) {
                next_node_->element_add(p);
            } else {
                this->father_add(p);
            }
        } else {
            auto save_key = this->max_key();

            if (elements_.add(pair)) { // if max_key changed
                // call BtreeHelper
                btree_->change_bound_upwards(this, save_key, this->max_key());
            }
        }
    }

    NODE_TEMPLATE
        void
        NODE_INSTANCE::father_add(const pair<Key, Value>& pair)
    {
        if (father_ == nullptr) {

        } else {
            // create new leaf
            auto n = new Node(this, leaf_type(), &pair, &pair + 1);
            // set next_node_
            next_node_ = n;
            // let father add
            father_->middle_append({ pair.first, n }, true);
        }
    }

//    template <typename T>
//    void operator=(const unique_ptr<T> lhs, const unique_ptr<T>& rhs)
//    {
//        return rhs;
//    }

#include <cstring>
    using namespace btree;
    using std::initializer_list;
    using std::pair;
    using std::vector;
    using std::unique_ptr;
    using std::memcpy;
#define NODE_TEMPLATE template <typename Key, typename Value, unsigned BtreeOrder, typename BtreeType>
#define NODE_INSTANCE Node<Key, Value, BtreeOrder, BtreeType>

    // public method part:

    // Common:
    // some methods are for middle, some for leaf

    NODE_TEMPLATE
        template <typename Iterator>
    NODE_INSTANCE::Elements::Elements(Iterator begin, Iterator end)
        : count_(0)
    {
        if (begin == end) {
            return;
        } else {
            do {
                Elements::assign(elements_[count_], *begin);

                ++count_;
                ++begin;
            } while (begin != end);
            this->reset_cache();
        }
    }

    NODE_TEMPLATE
        Key
        NODE_INSTANCE::Elements::max_key() const
    {
        return elements_[count_ - 1].first;
    }

    NODE_TEMPLATE
        bool
        NODE_INSTANCE::Elements::have(const Key& key)
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

    NODE_TEMPLATE
        vector<Key>
        NODE_INSTANCE::Elements::all_key() const {
        vector<Key> r(count_);
        for (size_t i = 0; i < count_; ++i) {
            r[i] = elements_[i].first;
        }
        return r;
    }

    NODE_TEMPLATE
        bool
        NODE_INSTANCE::Elements::full() const
    {
        return count_ >= BtreeOrder;
    }

    NODE_TEMPLATE
        bool
        NODE_INSTANCE::Elements::remove(const Key& key)
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
    NODE_TEMPLATE
        Value&
        NODE_INSTANCE::Elements::operator[](const Key& key)
    {
        // TODO
        // 1. should return ptr, check_out_digging need
        auto& cache_k = elements_[cache_index_].first;
        auto& cache_v = Elements::value(elements_[cache_index_].second);

        if (key == cache_k) {
            this->reset_cache();
            return cache_v;
        } else if (key < cache_k) {
            for (auto i = 0; i < cache_index_; ++i) {
                if (key == elements_[i].first) {
                    return Elements::value(elements_[i].second);
                }
            }
        } else {
            for (int i = cache_k; i < count_; ++i) {
                if (key == elements_[i].first) {
                    return Elements::value(elements_[i].second);
                }
            }
        }
    }

    // Node think first of full situation
    // return value indicate the the max-key is changed or not
    NODE_TEMPLATE
        template <typename T>
    bool
        NODE_INSTANCE::Elements::add(const pair<Key, T>& pair)
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

    NODE_TEMPLATE
        template <typename T>
    bool
        NODE_INSTANCE::Elements::append(const pair<Key, T>& pair)
    {
        elements_[count_] = pair;
        ++count_;
        return true;
    }

    NODE_TEMPLATE
        pair<Key, Value>
        NODE_INSTANCE::Elements::exchange_max_out(const pair<Key, Value>& p)
    {
        pair<Key, Value> max{ elements_[count_ - 1].first, Elements::value(elements_[count_ - 1].second) };
        for (auto& e : elements_) {
            if (p.first <= e.first) {
                auto pos = Elements::move_element(1, &e, &elements_[count_ - 2]);
                pos = p;
                return max;
            }
        }
        // TODO assert not arrive here
    }

    // for ptr
    NODE_TEMPLATE
        NODE_INSTANCE*
        NODE_INSTANCE::Elements::ptr_of_min() const
    {
        return Elements::ptr(elements_[0].second);
    }

    NODE_TEMPLATE
        NODE_INSTANCE*
        NODE_INSTANCE::Elements::ptr_of_max() const
    {
        return Elements::ptr(elements_[count_ - 1].second);
    }

    NODE_TEMPLATE
        bool
        NODE_INSTANCE::Elements::change_key(const Key& old_key, const Key& new_key)
    {
    }

    //NODE_TEMPLATE
    //template <typename T>
    //void
    //NODE_INSTANCE::Elements::add(const pair<Key, T*>& pair)
    //{
    //    elements_[count_].first = pair.first;
    //    elements_[count_].second.reset(pair.second);
    //    ++count_;
    //}
    // private method part:

    NODE_TEMPLATE
        Value
        NODE_INSTANCE::Elements::value(const std::variant<Value, std::unique_ptr<Node>>& v)
    {
        return std::get<Value>(v);
    }

    NODE_TEMPLATE
        NODE_INSTANCE*
        NODE_INSTANCE::Elements::ptr(const std::variant<Value, std::unique_ptr<Node>>& v)
    {
        return std::get<unique_ptr<Node>>(v).get();
    }

    NODE_TEMPLATE
        typename NODE_INSTANCE::Elements::content_type&
        NODE_INSTANCE::Elements::move_element(const char direction, content_type* begin, content_type* end)
    {
        memcpy(begin + direction, begin, end - begin);
        return *begin;
    }

    NODE_TEMPLATE
        typename NODE_INSTANCE::Elements::content_type&
        NODE_INSTANCE::Elements::to_end_move(const char direction, content_type* begin)
    {
        // default content_type* end
        auto end = &elements_[count_ - 1];
        return this->move_element(direction, begin, end);
    }

    // caller should ensure the key must exist
    NODE_TEMPLATE
        typename NODE_INSTANCE::Elements::content_type&
        NODE_INSTANCE::Elements::related_position(const Key& key)
    {
        // in the future, could use half search
        for (auto& e : elements_) {
            if (e.first == key) {
                return e;
            }
        }
    }

    NODE_TEMPLATE
    typename NODE_INSTANCE::Elements::content_type& 
        NODE_INSTANCE::Elements::assign(content_type& ele, const pair<Key, unique_ptr<Node>>& pair)
    {
        content_type tmp{ u_ptr.release() };
        ele = tmp;
        return ele;
    }

    NODE_TEMPLATE
        typename NODE_INSTANCE::Elements::content_type&
        NODE_INSTANCE::Elements::assign(content_type& ele, const pair<Key, Value>& pair)
    {

        return ele;
    }


    NODE_TEMPLATE
        void
        NODE_INSTANCE::Elements::reset_cache()
    {
        cache_index_ = count_ / 2;
    }

}
