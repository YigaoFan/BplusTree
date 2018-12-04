#include "pch.h"
#include <algorithm>
#include "Btree.h"
using std::ostream;
using std::endl;

Btree::Btree()
{
}



Btree::Btree(initializer_list<KeyType> valueList)
{
	for (auto value : valueList) {
		insert(value);
	}
}

Btree::~Btree()
{

}

ostream& operator<<(ostream& os, const Btree& item)
{
	//os << item.root.childCount << endl;
	return os;
}

// this is for insert method, maybe not suit for normal search
shared_ptr<Node> Btree::fit(PredicateFunc func)
{
	// for-each Node to chose which should be continued
	shared_ptr<Node> currentNode;
	for (currentNode = root;
			currentNode->hasChild(); // here ensure not explore the leaf
			currentNode = currentNode->giveMeTheWay(func));
	// will get the leaf node
	return currentNode;
}

int Btree::insert(KeyType e)
{
	std::cout << "insert key: " << e << endl;
	// expression below exist some problem need to be clear
	// the return value is the father?
	// using Currying to determine a argument
	shared_ptr<Node> parent = fit([e] (const Ele& ele) {
			if (ele.childValueLowBound > e) {
			return true;
			} else {
			return false;
			}});

	//todo: here I use the std::sort directly,
	//todo: in the future, update to use insert sort
	auto& eles = parent->getVectorOfEles();

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

void Btree::adjust()
{

}
