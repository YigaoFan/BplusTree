#pragma once

#include <vector>
// maybe template arg can be less, like BtreeType maybe reduce
template <typename Key, typename Value, unsigned BtreeOrder, typename BtreeType>
class NodeBase {
public:
	const bool middle;

	NodeBase(); // constructor
	virtual ~NodeBase();

	Node* next_node() const;
	void next_node(Node*);
};

