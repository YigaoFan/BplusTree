#include <algorithm>
#include "Btree.h"
using std::ostream;
using std::endl;

template <typename Key, typename Value, unsigned BtreeOrder>
Btree<Key, Value, BtreeOrder>::Btree(initializer_list<pair<Key, Value>> pairList)
{
	for (auto pair : pairList) {
		this->add(pair);
	}
}

template <typename Key, typename Value, unsigned BtreeOrder>
Btree<Key, Value, BtreeOrder>::~Btree()
{

}

// this is for add method, maybe not suit for normal search
template <typename Key, typename Value, unsigned BtreeOrder>
shared_ptr<Node>
Btree<Key, Value, BtreeOrder>::grabTheFitLeafBack(PredicateFunc func)
{
	// for-each Node to chose which should be continued
	shared_ptr<Node> currentNode;
	for (currentNode = root;
			currentNode->hasChild(); // here ensure not explore the leaf
			currentNode = currentNode->giveMeTheWay(func));
	// will get the leaf node
	return currentNode;
}

template <typename Key, typename Value, unsigned BtreeOrder>
int
Btree<Key, Value, BtreeOrder>::add(pair<Key, Value> e)
{
	std::cout << "insert key: " << e << endl;
	// expression below exist some problem need to be clear
	// the return value is the father?
	// using Currying to determine a argument
	shared_ptr<Node> parent = grabTheFitLeafBack([e] (const Ele& ele) {
			if (ele.childValueLowBound > e) {
			return true;
			} else {
			return false;
			}});

	//todo: here I use the std::sort directly,
	//todo: in the future, update to use add sort
	auto& eles = parent->getVectorOfElesRef();

	std::cout << "original: ";
	for (auto& leaf : eles) {
		std::cout << leaf.key << " ";
	}
	std::cout << endl;

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

	std::cout << "new: ";
	for (auto& leaf : eles) {
		std::cout << leaf.key << " ";
	}
	std::cout << endl;
	return 1;
}

template <typename Key, typename Value, unsigned BtreeOrder>
void
Btree<Key, Value, BtreeOrder>::adjust()
{

}
