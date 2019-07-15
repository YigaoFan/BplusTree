#pragma once
//#include "Proxy.hpp"
#include "NodeBaseCrtp.hpp"
#include "LeafNode.hpp"

namespace btree {
#define MIDDLE_NODE_TEMPLATE template <typename Key, typename Value, uint16_t BtreeOrder>
#define MIDDLE MiddleNode<Key, Value, BtreeOrder>

	MIDDLE_NODE_TEMPLATE
    class MiddleNode : public NodeBase_CRTP<MIDDLE, Key, Value, BtreeOrder> {
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

        Base* minSon();
        Base* maxSon();
		MiddleNode*      father() const override;
		bool             add(pair<Key, Value>&&) override;
        Base*            operator[](const Key&);
        pair<Key, Base*> operator[](uint16_t);

    private:
	    unique_ptr<Base> clone() const override;
    };
}

namespace btree {
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
    MIDDLE::minSon()
    {
	    auto& es = Base::elements_;

        return Base::Ele::ptr(es[0].second);
    }

    MIDDLE_NODE_TEMPLATE
    typename MIDDLE::Base*
    MIDDLE::maxSon()
    {
	    auto& es = Base::elements_;

        return Base::Ele::ptr(es[es.count() - 1].second);
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

    MIDDLE_NODE_TEMPLATE
	bool
	MIDDLE::add(pair<Key, Value>&& p)
	{
		auto& k = p.first;
		auto& v = p.second;

		for (auto& e : this->elements_) {
			if ((*this->elements_.LessThanPtr)(k, e.first)) {
				return Base::Ele::ptr(e.second)->add(std::move(p));
			}
		}

		// TODO wait to delete
		throw runtime_error("add pair encounter some error");
	}

	MIDDLE_NODE_TEMPLATE
    unique_ptr<typename MIDDLE::Base>
    MIDDLE::clone() const
    {
	    return make_unique<MIDDLE>(*this);
    }

#undef MIDDLE
#undef MIDDLE_NODE_TEMPLATE
}