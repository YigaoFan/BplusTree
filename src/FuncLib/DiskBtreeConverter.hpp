#pragma once
#include <memory>
#include <type_traits>
#include "../Btree/Basic.hpp"
#include "../Btree/Btree.hpp"
#include "../Btree/NodeBase.hpp"
#include "../Btree/LeafNode.hpp"
#include "../Btree/MiddleNode.hpp"

namespace FuncLib
{
	using ::std::shared_ptr;
	using ::std::declval;
	using ::std::decay_t;
	using ::Collections::order_int;
	using ::Collections::LessThan;
	using ::Collections::Btree;
	using ::Collections::NodeBase;
	using ::Collections::MiddleNode;
	using ::Collections::LeafNode;

	// DiskPtr should have this class as data member
	template <typename Key, typename Value, order_int Order>
	class DiskBtreeEnvironmentCarrier
	{
	private:
		using ThisType = DiskBtreeEnvironmentCarrier;
		using TreeType = Btree<Order, Key, Value>;
		using Node = NodeBase<Key, Value, Order>;
		using Mid = MiddleNode<Key, Value, Order>;
		using Leaf = LeafNode<Key, Value, Order>;
		shared_ptr<LessThan<Key>> _lessThanPtr;
		TreeType* _thisTreePtr;
	public:
		template <typename Key, typename Value, order_int Order>
		static DiskPtr<...>
		CreateDiskRootNode(decltype(declval<ThisType>()._lessThanPtr) lessThanPtr, decltype(declval<ThisType>()._thisTreePtr) treePtr)
		{

		}

		DiskBtreeEnvironmentCarrier(decltype(_lessThanPtr) lessThanPtr, decltype(_thisTreePtr) treePtr)
			: _lessThanPtr(lessThanPtr), _thisTreePtr(treePtr)
		{ }

		template <typename T>
		auto ConvertFromDisk(uint32_t startInFile)
		{
			using type = decay_t<T>;
			return DoConvertFromDisk(startInFile, static_cast<type*>(nullptr));
		}

		shared_ptr<Node> DoConvertFromDisk(uint32_t startInFile, Node*)
		{

		}

		shared_ptr<Leaf> DoConvertFromDisk(uint32_t startInFile, Leaf*)
		{

		}

		shared_ptr<Mid> DoConvertFromDisk(uint32_t startInFile, Mid*)
		{

		}


	private:

	};

	
}