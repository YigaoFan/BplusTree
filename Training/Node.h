#pragma once // ensure included once

#include <array> // for array
#include <memory> // for shared_ptr
#include <utility> // for pair
#include "CommonFlag.h" // for project define marco
#include "NodeIter.h" // for NodeIter

namespace btree {

    template <typename Key, typename Value>
    class Ele {
    public:
        // todo: complete the construct control, this and derived class
        Ele();
        ~Ele();
        virtual Key& key();
    private:
        virtual void* __value();
    };

    template <typename Key,
        typename Value>
        class ValueEle : public Ele<Key, Value> {
        private:
            std::pair<Key, Value> leaf_;
        public:
            Key& key() override
            { return leaf_.first; }
            Value& value()
            { return *__value(); }
        private:
            void* __value() override
            { return &(leaf_.second); }
    };

    template <typename Key,
        typename Value,
        typename NodeType,
        typename BtreeType>
        class PointerEle : Ele<Key, Value> {
        private:
            // todo: or use shared_ptr<NodeType>
            std::pair<Key, NodeType*>leaf_;
        public:
            Key& key() override
            { return leaf_.first; }
            NodeType* pointer()
            { return (NodeType*)__value(); }
        private:
            void* __value() override
            { return &(leaf_.second); }
    };

    struct leaf_type {};
    struct middle_type {};
    // When a Node is Created, its all type is done!
	template<typename Key, typename Value, unsigned BtreeOrder, typename BtreeType>
	class Node {
    private:
        using ele_instance_type = Ele<Key, Value>;

	public:
        // Property
        const bool middle;

        // Construct
        explicit Node(const bool&, const BtreeType*, const Node*, const leaf_type);
        explicit Node(const bool&, const BtreeType*, const Node*, const middle_type);
        Node(const bool&, const BtreeType*, const std::pair<Key, Value>&, const Node*);
        // todo: maybe code the up construct method in ValueEle and PointerEle way
        ~Node();

		// Iterator
		NodeIter<ele_instance_type> begin();
		NodeIter<ele_instance_type> end();

        // Method
		std::shared_ptr<ele_instance_type> operator[](const Key&&); // todo: can use &&
        RESULT_FLAG add(const std::pair<Key, Value>&);
        void remove(const Key&);

	private:
		// Field
        // todo: maybe change to vector
		std::array<std::shared_ptr<ele_instance_type>, BtreeOrder> elements_; // todo: construct
        decltype(elements_.size()) elements_count_{0};
        std::shared_ptr<Node> next_node_{nullptr};  // todo: complete
        const BtreeType* btree_;
        Node* father_;

        // Helper 
        //bool has_child() { return middle && elements_count_ > 0; }
        bool full() { return elements_count_ >= BtreeOrder; }
            // for add
        RESULT_FLAG no_area_add(std::pair<Key, Value>);
        RESULT_FLAG area_add(const std::pair<Key, Value>&);
        RESULT_FLAG middle_node_add(const std::pair<Key, Value>&);
            // for add and remove
        void move_Ele(const NodeIter<ele_instance_type>&, const NodeIter<ele_instance_type>&,
                              unsigned=1);
        void adjust();

        // Old function
        //RESULT_FLAG insert(std::shared_ptr<ele_instance_type>);
        void do_insert(std::shared_ptr<ele_instance_type>);
        void exchange_the_biggest_ele_out(std::shared_ptr<ele_instance_type>);
        // make the method below like this createNewRoot(node1, node2...);
        static RESULT_FLAG createNewRoot(const std::shared_ptr<Node>&, const std::shared_ptr<ele_instance_type> &);
        static std::shared_ptr<ele_instance_type> constructToMakeItFitTheFatherInsert(std::shared_ptr<ele_instance_type>);
	};
}
