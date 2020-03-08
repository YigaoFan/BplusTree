#pragma once
/***********************************************************************************************************
   Tree Inspector in Collections
***********************************************************************************************************/
#include <vector>
#include <iostream>
#include "NodeBase.hpp"

namespace Collections
{
	using ::std::cout;
	using ::std::endl;

	template <typename Key, typename Value, order_int BtreeOrder>
	auto MoveDeep(vector<NodeBase<Key, Value, BtreeOrder>*> deepNodes) -> decltype(deepNodes)
	{
		auto deepers = decltype(deepNodes)();
		for (auto n : deepNodes)
		{
			for (auto sn : n->SubNodes())
			{
				deepers.push_back(sn);
			}
		}

		return deepers;
	}

	template <typename Key, typename Value, order_int BtreeOrder>
	void InspectNodeKeys(NodeBase<Key, Value, BtreeOrder>* node)
	{
		auto deep = 0;
		auto deepNodes = vector<decltype(node)>{ node, };
		do
		{
			for (auto n : deepNodes)
			{
				for (auto& k : n->SubNodeMinKeys())
				{
					cout << k << " ";
				}

				cout << "  ";
			}

			cout << endl;
			deepNodes = MoveDeep(deepNodes);
		} while (deepNodes.size() != 0);
	}
}