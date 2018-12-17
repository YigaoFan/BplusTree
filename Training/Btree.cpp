#include <algorithm>
#include "Btree.h"
// seem like the type-para is not useful in this file
#define BTREE_TEMPLATE_DECLARATION template <typename Key, typename Value, unsigned BtreeOrder>
#define BTREE_INSTANCE Btree<Key, Value, BtreeOrder>
using std::ostream;
using std::endl;

// pulic method part:
BTREE_TEMPLATE_DECLARATION
BTREE_INSTANCE::Btree(initializer_list<pair<Key, Value>> pair_list)
{
	for (auto& pair : pair_list) {
		if (check(pair.first)) {
			this->add(pair);
		}
	}
}

BTREE_TEMPLATE_DECLARATION
BTREE_INSTANCE::~Btree()
{

}

BTREE_TEMPLATE_DECLARATION
int
BTREE_INSTANCE::add(pair<Key, Value> e)
{
	// expression below exist some problem need to be clear
	// the return value is the father?
	// using Currying to determine a argument
	shared_ptr<NodeType> parent = getFitLeafBack([e] (const Ele& ele) {
			if (ele.childValueLowBound > e) {
			return true;
			} else {
			return false;
			}});

	//todo: here I use the std::sort directly,
	//todo: in the future, update to use add sort
	auto& eles = parent->getVectorOfElesRef();

	eles.push_back(e);
	std::sort(eles.begin(), eles.end(),
			[](const Ele& ele1, const Ele& ele2)
			{
			if (ele1.key < ele2.key) {
			return true;
			} else {
			return false;
			}
			});

	return 1;
}

// private method part:
BTREE_TEMPLATE_DECLARATION
void
BTREE_INSTANCE::adjust()
{

}

BTREE_TEMPLATE_DECLARATION
bool
BTREE_INSTANCE::check(Key key)
{

}

// helper for add method
BTREE_TEMPLATE_DECLARATION
shared_ptr<NodeType>
BTREE_INSTANCE::getFitLeafBack(PredicateFunc func)
{
	// for-each Node to chose which should be continued
	shared_ptr<NodeType> currentNode;
	for (currentNode = root;
		currentNode->hasChild(); // here ensure not explore the leaf
		currentNode = currentNode->giveMeTheWay(func));
	// will get the leaf node
	return currentNode;
}
