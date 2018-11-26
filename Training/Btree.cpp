//#include "pch.h"
#include "Btree.h"
using std::ostream;
using std::endl;

Btree::Btree()
{
}

//Btree::Btree(ValueType e1, ValueType e2, ...)
//{
//	// how to insert no
//}

Btree::~Btree()
{

}

ostream& operator<<(ostream& os, const Btree& item)
{
    //os << item.root.childCount << endl;
    return os;
}

vector<Node*> Btree::find(bool (* func)(const Node&))
{
    // for-each Node to chose which should be continued
    for (Node* currentNode = root;
    currentNode != nullptr;
    currentNode = currentNode->giveMeTheWay(func));



}

int Btree::insert(ValueType e)
{
    // expression below exist some problem need to be clear
    // the return value is the father?
    // using Currying to determine a argument
    vector<Node*> result = find([] (const Node& currentNode)
                                {
                                    return true;
                                });
    Node* insertParent = result[0];

    return 1;
}
