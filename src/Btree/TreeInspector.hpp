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

	template <typename Key, typename Value, order_int Order, StorePlace Place>
	auto MoveDeep(vector<typename NodeBase<Key, Value, Order, Place>::template OwnerLessPtr<NodeBase<Key, Value, Order, Place>>> deepNodes) -> decltype(deepNodes)
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

	template <typename Key, typename Value, order_int Order, StorePlace Place>
	void InspectNodeKeys(typename NodeBase<Key, Value, Order, Place>::template OwnerLessPtr<NodeBase<Key, Value, Order, Place>> node)
	{
		// if (not CheckCompliance<Key, Value, Order, Place>(node))
		{
			// cout << "Something wrong in tree:" << endl;
			auto deepNodes = vector<decltype(node)>{ node, };
			do
			{
				for (auto n : deepNodes)
				{
					for (auto &k : n->KeysInThisNode())
					{
						cout << k << " ";
					}

					cout << "  ";
				}

				cout << endl;
				deepNodes = MoveDeep<Key, Value, Order, Place>(deepNodes);
			} while (deepNodes.size() != 0);
		}
	}

	template <typename Key, typename Value, order_int Order, StorePlace Place>
	bool CheckCompliance(typename NodeBase<Key, Value, Order, Place>::template OwnerLessPtr<NodeBase<Key, Value, Order, Place>> node)
	{
		auto ks = node->KeysInThisNode();
		if (ks.size() < node->LowBound and ks.size() > Order)
		{
			return false;
		}

		for (auto n : node->SubNodes())
		{
			if (not CheckCompliance<Key, Value, Order, Place>(n))
			{
				return false;
			}
		}

		return true;
	}
}