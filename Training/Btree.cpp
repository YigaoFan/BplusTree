#include "pch.h"
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

Node* Btree::find(PredicateFunc func)
{
	if (!root.hasChild()) { return nullptr; }
    // for-each Node to chose which should be continued
	Node* currentNode;
    for (currentNode = &root;
    currentNode->hasChild(); // here ensure not explore the leaf
    currentNode = currentNode->giveMeTheWay(func));

	return currentNode;
}

int Btree::insert(KeyType e)
{
    // expression below exist some problem need to be clear
    // the return value is the father?
    // using Currying to determine a argument
    Node& result = *find([] (const Ele& currentnode)
                                {
                                    return true;
                                });
    

    return 1;
}
