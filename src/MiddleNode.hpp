#pragma once
#include "NodeBaseCrtp.hpp"

namespace btree {
#define NODE_TEMPLATE template <typename Key, typename Value, uint16_t BtreeOrder>
#define MIDDLE MiddleNode<Key, Value, BtreeOrder>

	NODE_TEMPLATE
	class MiddleNode : public NodeBase_CRTP<MIDDLE, Key, Value, BtreeOrder> {
	private:
		using          Base = NodeBase<Key, Value, BtreeOrder>;
		using          Base_CRTP = NodeBase_CRTP<MIDDLE, Key, Value, BtreeOrder>;
		using typename Base::LessThan;

	public:
		template <typename Iter>
		MiddleNode(Iter, Iter, shared_ptr<LessThan>);
		MiddleNode(const MiddleNode&);
		MiddleNode(MiddleNode&&) noexcept;
		~MiddleNode() override;

		Base* minSon() const;
		void  searchSiblings(vector<Base *> &, Base *&, Base *&) const;

	private:
		Base* maxSon() const;
	};
}

namespace btree {
	NODE_TEMPLATE
	template <typename Iter>
	MIDDLE::MiddleNode(Iter begin, Iter end, shared_ptr<LessThan> funcPtr)
		: Base_CRTP(begin, end, funcPtr)
	{ }

	NODE_TEMPLATE
	MIDDLE::MiddleNode(const MiddleNode& that)
		: Base_CRTP(that)
	{ }

	NODE_TEMPLATE
	MIDDLE::MiddleNode(MiddleNode&& that) noexcept
		: Base_CRTP(std::move(that))
	{ }

	NODE_TEMPLATE
	MIDDLE::~MiddleNode() = default;

	NODE_TEMPLATE
	typename MIDDLE::Base*
	MIDDLE::minSon() const
	{
		auto& es = Base::elements_;

		return Base::Ele::ptr(es[0].second);
	}

	/**
	 * not change the stack
	 */
	NODE_TEMPLATE
	void
	MIDDLE::searchSiblings(vector<Base *> &passedNodeTrackStack, Base *&previous, Base *&next) const
	{
		auto& stack = passedNodeTrackStack;

		auto rIter = stack.rbegin();
		auto rEnd  = stack.rend();

		// Some don't have sibling in Middle?
		function<Base*(decltype(rIter))> searchPreHelper = [&] (decltype(rIter)) -> Base* {
			auto currentNodeIter = rIter;

			auto upperNodeIter = ++rIter;
			auto& upperElements = upperNodeIter->elements_;
			auto i = upperElements.indexOf(currentNodeIter);

			if (i > 0) {
				return Base::Ele::ptr(upperElements[i-1].second);
			} else {
				if (upperNodeIter == rEnd) {
					return nullptr;
				}

				// see in the upper node, this is not min
				auto previousOfUpper = searchPreHelper(upperNodeIter);
				return previousOfUpper->maxSon();
			}
		};

		previous = searchPreHelper(rIter);

		// reset
		rIter = stack.rbegin();

		function<Base*(decltype(rIter))> searchNxtHelper = [&] (decltype(rIter)) -> Base* {
			auto currentNodeIter = rIter;
			auto upperNodeIter = ++rIter;
			auto& upperElements = upperNodeIter->elements_;
			auto i = upperElements.indexOf(currentNodeIter);

			auto maxI = upperNodeIter->childCount() - 1;
			// previous and next won't need recur all at one searchSiblings call
			if (i < maxI) {
				return Base::Ele::ptr(upperElements[i+1].second);
			} else {
				if (upperNodeIter == rEnd) {
					return nullptr;
				}

				// see in the upper node, this is not max
				auto previousOfUpper = searchNxtHelper(upperNodeIter);
				return previousOfUpper->maxSon();
			}
		};

		next = searchNxtHelper(rIter);


	}

#undef MIDDLE
#undef NODE_TEMPLATE
}