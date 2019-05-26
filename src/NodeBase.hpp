#pragma once
#include <vector> 		// for vector
#include "Elements.hpp" // for Elements
//#include "Proxy.hpp"

namespace btree {
    struct leafType {};
    struct middleType {};

#define NODE_BASE_TEMPLATE template <typename Key, typename Value, uint16_t BtreeOrder>

    NODE_BASE_TEMPLATE
    class NodeBase {
    public:
		using Ele         = Elements<Key, Value, BtreeOrder, NodeBase>;
		using CompareFunc = typename Ele::CompareFunc;
        const bool Middle;

        template <typename Iter>
        NodeBase(leafType, Iter, Iter, shared_ptr<CompareFunc>);
        template <typename Iter>
        NodeBase(middleType,Iter, Iter, shared_ptr<CompareFunc>);
        NodeBase(const NodeBase&);
        NodeBase(NodeBase&&) noexcept;
        virtual ~NodeBase();

        inline Key         maxKey() const;
        inline bool        have(const Key&);
        inline vector<Key> allKey() const;

		void              father(NodeBase*);
        virtual NodeBase* father() const;

		uint16_t    childCount() const;
		inline bool empty() const;
		inline bool full() const;
    protected:
        NodeBase* father_{ nullptr };
        Elements<Key, Value, BtreeOrder, NodeBase> elements_;
    };
}

// implementation
namespace btree {
#define NODE NodeBase<Key, Value, BtreeOrder>

	NODE_BASE_TEMPLATE
    template <typename Iter>
    NODE::NodeBase(leafType, Iter begin, Iter end, shared_ptr<CompareFunc> funcPtr)
        : Middle(false), elements_(begin, end, funcPtr)
    {}

	NODE_BASE_TEMPLATE
    template <typename Iter>
    NODE::NodeBase(middleType, Iter begin, Iter end, shared_ptr<CompareFunc> funcPtr)
        : Middle(true), elements_(begin, end, funcPtr)
    {
        for (; begin != end; ++begin) {
            // need to use SFINE to control the property below?
            (*begin)->father_ = this;
        }
    }

    NODE_BASE_TEMPLATE
    NODE::NodeBase(const NodeBase& that)
        : Middle(that.Middle), elements_(that.elements_)
    {}

    NODE_BASE_TEMPLATE
	NODE::NodeBase(NodeBase&& that) noexcept
		: Middle(std::move(that.Middle)), father_(that.father_), elements_(std::move(that.elements_))
	{}


	NODE_BASE_TEMPLATE
    NODE*
    NODE::father() const
    {
    	return father_;
    }

    NODE_BASE_TEMPLATE
    void
    NODE::father(NodeBase* father)
    {
		father_ = father;
    }

    NODE_BASE_TEMPLATE
	uint16_t
    NODE::childCount() const
    {
		return elements_.count();
    }

	NODE_BASE_TEMPLATE
	bool
	NODE::empty() const
	{
		return elements_.count() == 0;
	}

	NODE_BASE_TEMPLATE
	bool
	NODE::full() const
	{
		return elements_.full();
	}

    NODE_BASE_TEMPLATE
    NODE::~NodeBase() = default;

//    template <typename Key, typename Value, int16_t BtreeOrder, typename BtreeType>
//    Proxy<Key, Value, BtreeOrder, BtreeType>
//    NodeBase<Key, Value, BtreeOrder, BtreeType>::self()
//    {
//       return this;
//    }

	NODE_BASE_TEMPLATE
    Key
    NODE::maxKey() const
    {
        return elements_.rightMostKey();
    }

    NODE_BASE_TEMPLATE
    bool
    NODE::have(const Key& key)
    {
        return elements_.have(key);
    }

    NODE_BASE_TEMPLATE
    vector<Key>
    NODE::allKey() const
    {
        return elements_.allKey();
    }

#undef NODE
#undef NODE_BASE_TEMPLATE
}
