#pragma once
//#include "Proxy.hpp"
#include "NodeBaseCrtp.hpp"
// #include "LeafNode.hpp"

namespace btree {
#define NODE_TEMPLATE template <typename Key, typename Value, uint16_t BtreeOrder>
#define MIDDLE MiddleNode<Key, Value, BtreeOrder>

	NODE_TEMPLATE
    class MiddleNode : public NodeBase_CRTP<MIDDLE, Key, Value, BtreeOrder> {
    private:
        using          Base = NodeBase<Key, Value, BtreeOrder>;
        using typename Base::LessThan;

    public:
        template <typename Iter>
        MiddleNode(Iter, Iter, shared_ptr<LessThan>);
        MiddleNode(const MiddleNode&);
		MiddleNode(MiddleNode&&) noexcept;
        ~MiddleNode() override;

        Base* minSon();
        // Base* maxSon();
		MiddleNode*      father() const override;
		bool             add(pair<Key, Value>&&);
        Base*            operator[](const Key&);
        pair<Key, Base*> operator[](uint16_t);

    private:
        void addBeyondMax(pair<Key, Value>);
    };
}

namespace btree {
	NODE_TEMPLATE
    template <typename Iter>
    MIDDLE::MiddleNode(Iter begin, Iter end, shared_ptr<LessThan> funcPtr)
        : Base(MiddleFlag(), begin, end, funcPtr)
    { }

    NODE_TEMPLATE
    MIDDLE::MiddleNode(const MiddleNode& that)
		: Base(that) // TODO wrong work
	{
		using Base::Ele::ptr;

		for (auto& e : Base::elements_) {
			// Set to corresponding this elements_ position
			ptr(e.second)->clone();
		}
	}

	NODE_TEMPLATE
	MIDDLE::MiddleNode(MiddleNode&& that) noexcept
		: Base(std::move(that))
	{ }

    NODE_TEMPLATE
    MIDDLE::~MiddleNode() = default;

	NODE_TEMPLATE
    typename MIDDLE::Base*
    MIDDLE::minSon()
    {
	    auto& es = Base::elements_;

        return Base::Ele::ptr(es[0].second);
    }

    NODE_TEMPLATE
    typename MIDDLE::Base*
    MIDDLE::operator[](const Key& key)
    {
    	auto& e = this->elements_[key];
    	return Base::Ele::ptr(e);
    }

	NODE_TEMPLATE
	pair<Key, typename MIDDLE::Base*>
	MIDDLE::operator[](uint16_t i)
	{
		auto& e = this->elements_[i];
		return make_pair(e.first, Base::Ele::ptr(e.second));
	}

	NODE_TEMPLATE
    MIDDLE*
    MIDDLE::father() const
    {
        return static_cast<MiddleNode*>(Base::father());
    }

    NODE_TEMPLATE
	bool
	MIDDLE::add(pair<Key, Value>&& p)
	{
		// auto& k = p.first;
		// auto& v = p.second;

		// for (auto& e : this->elements_) {
		// 	if ((*this->elements_.LessThanPtr)(k, e.first)) {
		// 		return Base::Ele::ptr(e.second)->add(std::move(p));
		// 	}
		// }

		// // TODO wait to delete
		// throw runtime_error("add pair encounter some error");
        using Base::elements_;
		using Base::Ele::ptr;

        auto& k = p.first;
		auto& lessThan = *(elements_.LessThanPtr);

        for (auto &e : elements_) {
            if (lessThan(k, e.first)) {
                auto subNodePtr = ptr(e.second);
                subNodePtr->add(p);
            }
        }

        addBeyondMax(p);
    }

    // Name maybe not very accurate, because not must beyond
	NODE_TEMPLATE
	void
	MIDDLE::addBeyondMax(pair<Key, Value> p)
	{
        using Base::elements_;
        using Base::Ele::ptr;
		// Ptr append will some thing different
		if (!Base::full()) {
			elements_.append(p);
            // update bound related info
		} else if (/*sibling space free*/) {
			// move some element to sibling
		} else {
			// split node into two
		}
		// Think about this way: should add new right most node to expand max bound

	}

#undef MIDDLE
#undef NODE_TEMPLATE
}