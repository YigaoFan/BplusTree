#pragma once
/***********************************************************************************************************
   Universal Enumerator in Collections
***********************************************************************************************************/
#include <vector>
#include "Basic.hpp"
#include "Btree.hpp"

namespace Collections
{
    using ::std::vector;

    template <order_int BtreeOrder, typename Key, typename Value>
    class UniversalEnumerator
    {
    private:
        Btree<BtreeOrder, Key, Value>& _btree;
        vector<NodeBase*> _deepStack;
    public:
        UniversalEnumerator(decltype(_btree)& btree)
            : _btree(btree)
        { }

        bool MoveUp(NodeBase*& nextNode)
        {
            if (_deepStack.size() > 0)
            {
				_deepStack.pop_back();
                nextNode = _deepStack.back();
                return true;
            }

            return false;
        }

        // 有点复杂，万一 MoveDown 之后，还 MoveRight 了，怎么 MoveUp ?

        bool MoveDown(NodeBase*& nextNode)
        {
            // TODO add root in stack
            auto successfully = _btree._root->MoveDown(nextNode);
            _deepStack.push_back(nextNode);
            return successfully;
        }

        bool MoveLeft(NodeBase*& nextNode)
        {
            return _btree._root->MoveLeft(nextNode);
        }

        bool MoveRight(NodeBase*& nextNode)
        {
            return _btree._root->MoveRight(nextNode);
        }
    };
}