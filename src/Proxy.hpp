#pragma once

namespace btree {
	template <typename Key, typename Value, int16_t BtreeOrder, typename BtreeType>
	class NodeBase;
	template <typename Key, typename Value, int16_t BtreeOrder, typename BtreeType>
	class MiddleNode;
	template <typename Key, typename Value, int16_t BtreeOrder, typename BtreeType>
	class LeafNode;

	template <typename Key, typename Value, int16_t BtreeOrder, typename BtreeType>
	class Proxy {
	private:
		using B = NodeBase<Key, Value, BtreeOrder, BtreeType>;
		using M = MiddleNode<Key, Value, BtreeOrder, BtreeType>;
		using L = LeafNode<Key, Value, BtreeOrder, BtreeType>;
		B* p;

	public:

		explicit Proxy(B* p)
			: p(p) {};
		operator M*() { return static_cast<M*>(p); }
		operator L*() { return static_cast<L*>(p);}
	};
}
