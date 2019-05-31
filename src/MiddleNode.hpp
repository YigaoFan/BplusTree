#pragma once
//#include "Proxy.hpp"
#include "NodeBase.hpp"
#include "LeafNode.hpp"

namespace btree {
#define MIDDLE_NODE_TEMPLATE template <typename Key, typename Value, uint16_t BtreeOrder>

	MIDDLE_NODE_TEMPLATE
    class MiddleNode final : public NodeBase<Key, Value, BtreeOrder> {
    private:
        using          Base = NodeBase<Key, Value, BtreeOrder>;
        using typename Base::LessThan;

    public:
        template <typename Iter>
        MiddleNode(Iter, Iter, shared_ptr<LessThan>);
        // should use another constructor for Leaf
        MiddleNode(const MiddleNode&);
		MiddleNode(MiddleNode&&) noexcept;
        ~MiddleNode() override;

        Base* minSon() const;
        Base* maxSon() const;
		MiddleNode*    father() const override;
        Base*            operator[](const Key&);
        pair<Key, Base*> operator[](uint16_t);
    };
}

// implement
namespace btree {
#define MIDDLE MiddleNode<Key, Value, BtreeOrder>

	MIDDLE_NODE_TEMPLATE
    template <typename Iter>
    MIDDLE::MiddleNode(Iter begin, Iter end, shared_ptr<LessThan> funcPtr)
        : Base(MiddleFlag(), begin, end, funcPtr)
    {}

    MIDDLE_NODE_TEMPLATE
    MIDDLE::MiddleNode(const MiddleNode& that)
		: Base(that)
	{}

	MIDDLE_NODE_TEMPLATE
	MIDDLE::MiddleNode(MiddleNode&& that) noexcept
		: Base(std::move(that))
	{}

    MIDDLE_NODE_TEMPLATE
    MIDDLE::~MiddleNode() = default;

	MIDDLE_NODE_TEMPLATE
    typename MIDDLE::Base*
    MIDDLE::minSon() const
    {
        return this->elements_.ptrOfMin();
    }

    MIDDLE_NODE_TEMPLATE
    typename MIDDLE::Base*
    MIDDLE::maxSon() const
    {
        return this->elements_.ptrOfMax();
    }

    MIDDLE_NODE_TEMPLATE
    typename MIDDLE::Base*
    MIDDLE::operator[](const Key& key)
    {
    	auto& e = this->elements_[key];
    	return Base::Ele::ptr(e);
    }

	MIDDLE_NODE_TEMPLATE
	pair<Key, typename MIDDLE::Base*>
	MIDDLE::operator[](uint16_t i)
	{
		auto& e = this->elements_[i];
		return make_pair(e.first, Base::Ele::ptr(e.second));
	}

	MIDDLE_NODE_TEMPLATE
    MIDDLE*
    MIDDLE::father() const
    {
        return static_cast<MiddleNode*>(Base::father());
    }

//    MIDDLE_NODE_TEMPLATE
//    Proxy<Key, Value, BtreeOrder>
//    MIDDLE::self()
//    {
//        return Proxy{this};
//    }
#undef MIDDLE
#undef MIDDLE_NODE_TEMPLATE
}