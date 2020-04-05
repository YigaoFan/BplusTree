#pragma once
#include <type_traits>
#include <vector>
#include <array>
#include <string>
#include <memory>
#include <cstddef>
#include "../Btree/NodeBase.hpp"
#include "../Btree/MiddleNode.hpp"
#include "../Btree/LeafNode.hpp"
#include "../Btree/Btree.hpp"
#include "../Btree/Elements.hpp"
#include "../Btree/LiteVector.hpp"
#include "../Btree/Basic.hpp"
#include "DiskAllocator.hpp"
#include "DiskPtr.hpp"
#include "CurrentFile.hpp"

namespace FuncLib
{
	using ::std::is_trivial_v;
	using ::std::is_standard_layout_v;
	using ::std::memcpy;
	using ::std::vector;
	using ::std::array;
	using ::std::string;
	using ::std::shared_ptr;
	using ::std::make_shared;
	using ::std::byte;
	using ::std::size_t;
	using ::Collections::Btree;
	using ::Collections::NodeBase;
	using ::Collections::LeafNode;
	using ::Collections::MiddleNode;
	using ::Collections::Elements;
	using ::Collections::LiteVector;
	using ::Collections::order_int;
	using ::Collections::LessThan;
	
	enum ToPlace
	{
		Stack,
		Heap,
	};

	template <typename T>
	struct ReturnType;

	template <typename R, typename... Args>
	struct ReturnType<R(Args...)>
	{
		using Type = R;
	};

	constexpr size_t Min(size_t one, size_t two)
	{
		return one > two ? one : two;
	}

	template <typename T>
	struct DiskDataConverter
	{
		static_assert(is_trivial_v<T> && is_standard_layout_v<T>,
			"Only support type which can use memory copy directly, "
			"Write a specialization for this template class to convert it to copyable");

		static array<byte, sizeof(T)> ConvertToDiskData(T t)
		{
			array<byte, sizeof(T)> mem;
			memcpy(&mem, &t, sizeof(T));
			return mem;
		}

		template <ToPlace Place = ToPlace::Heap>
		static auto ConvertFromDiskData(uint32_t startInFile)
		{
			array<byte, sizeof(T)> raw = CurrentFile::Read<T>(startInFile, sizeof(T));
			T* p = reinterpret_cast<T*>(&raw);

			if constexpr (Place == ToPlace::Stack)
			{
				return *p;
			}
			else
			{
				return make_shared<T>(*p);
			}
		}
	};

	template <>
	struct DiskDataConverter<string>
	{
		// TODO return value could be iterated
		vector<byte> ConvertToDiskData(string const& t)
		{
			return { t.begin(), t.end() };
		}

		shared_ptr<string> ConvertFromDiskData(uint32_t startInFile)
		{
			auto size = CurrentFile::Read<uint32_t>(startInFile, sizeof(uint32_t));
			auto contentStart = startInFile + sizeof(uint32_t);
			auto chars = CurrentFile::Read(contentStart, size);
			return make_shared<string>(chars.begin(), chars.end());
		}
	};

	using LibTree = Btree<3, string, string>;
	using LibTreeLeaf = LeafNode<string, string, 3>;
	using LibTreeLeafEle = Elements<string, string, 3>;
	using LibTreeMid = MiddleNode<string, string, 3>;
	using LibTreeMidEle = Elements<string, string, 3>;

	template <typename Key, typename Value, order_int Order>
	struct DiskDataConverter<Btree<Order, Key, Value>>
	{
		static array<byte, UnitSize> ConvertToDiskData(Middle& t)
		{
			auto middle = true;
		}

		static shared_ptr<Middle> ConvertFromDiskData(uint32_t startInFile)
		{

		}
	};

	// T is the type which could directly copy into disk
	template <typename T, typename size_int, size_int Capacity>
	struct DiskDataConverter<LiteVector<T, size_int, Capacity>>
	{
		using ThisType = LiteVector<T, size_int, Capacity>;

		static
		array<byte, sizeof(Capacity) + sizeof(T) * Capacity>
		ConvertToDiskData(ThisType const& vec)
		{
			array<byte, sizeof(Capacity) + sizeof(T) * Capacity> mem;

			// Count
			auto c = vec.Count();
			auto countSize = sizeof(Capacity);
			memcpy(&mem, &c, countSize);
			// Items
			for (auto i = 0; i < vec.Count(); ++i)
			{
				auto size = sizeof(T);
				auto s = &vec[i];
				auto d = &mem[i * size + countSize];
				memcpy(d, s, size);
			}

			return mem;
		}

		template <ToPlace Place = ToPlace::Heap>
		static auto ConvertFromDiskData(uint32_t startInFile)
		{
			if constexpr (Place == ToPlace::Stack)
			{

			}
			else
			{
				//shared_ptr<ThisType>
			}
		}
	};

	// This not same to LiteVector
	// , not to Key, Value is the type which can be copied into disk directly
	template <typename Key, typename Value, order_int Order>
	struct DiskDataConverter<Elements<Key, Value, Order>>
	{
		using ThisType = Elements<Key, Value, Order>;
		// CompileIf is duplicate in MiddleNode.hpp
		template <bool Value, typename A, typename B>
		struct CompileIf;

		template <typename A, typename B>
		struct CompileIf<true, A, B>
		{
			using Result = A;
		};

		template <typename A, typename B>
		struct CompileIf<false, A, B>
		{
			using Result = B;
		};

		template <typename T>
		// TODO below condition should judge if dynamiclly increase memory usage
		using Convert = CompileIf<is_trivial_v<T>&& is_standard_layout_v<T>, T, DiskPtr<T>>;

		static auto ConvertToDiskData(ThisType& t)
		{
			//struct Item
			//{
			//	// if pair content is POD, directly move to here
			//	// else replace with DiskPtr
			//	// recursive
			//	Key Key;
			//	Value Value;
			//};

			// Converted Key and Value
			// If type has heap memory, convert to DiskPtr<T> type
			// or convert to POD type
			auto ck = ConvertToDiskData(Key);
			auto cv = ConvertToDiskData(Value);
			struct Item
			{
				Convert<Key> Key;
				Convert<Value> Value;
			};
			LiteVector<Item, order_int, Order> vec;
			for (auto& i : t)
			{
				vec.Add({i.first, i.second});
			}

			return DiskDataConverter<Item>::ConvertToDiskData(vec);
		}

		static shared_ptr<ThisType> ConvertFromDiskData(uint32_t startInFile, shared_ptr<LessThan<Key>> lessThanPtr)
		{
			// Each type should have a constructor of all data member to easy set

			// Read LiteVector
			// And set LessThan
		}
	};

	template <typename Key, typename Value, auto Count>
	struct DiskDataConverter<MiddleNode<Key, Value, Count>>
	{
		using Middle = LibTreeMid;

		// TODO should be POD
		struct DiskMid
		{
			bool Middle;
			void* UpPtr;
			ReturnType<decltype(DiskDataConverter<LibTreeMidEle>::ConvertToDiskData)>::Type Elements;
		};

		using Converted = DiskMid;

		static array<byte, UnitSize> ConvertToDiskData(Middle& t)
		{
			return DiskDataConverter<DiskMid>::ConvertToDiskData({ true, nullptr, });
		}

		static shared_ptr<Middle> ConvertFromDiskData(uint32_t startInFile)
		{

		}
	};

	template <typename Key, typename Value, auto Count>
	struct DiskDataConverter<LeafNode<Key, Value, Count>>
	{
		using Leaf = LibTreeLeaf;
		struct DiskLeaf
		{

		};

		using Converted = DiskLeaf;
		static array<byte, UnitSize> ConvertToDiskData(Leaf& t)
		{
			auto middle = false;
			CurrentFile::Write<bool>();

		}

		static shared_ptr<Leaf> ConvertFromDiskData(uint32_t startInFile)
		{
		}
	};

	template <typename Key, typename Value, order_int Count>
	struct DiskDataConverter<NodeBase<Key, Value, Count>>
	{
		using Node = NodeBase<string, string, Count>;
		using MidNode = MiddleNode<string, string, Count>;
		using LeafNode = LeafNode<string, string, Count>;

		static array<byte, UnitSize> ConvertToDiskData(Node& node)
		{
			if (node.Middle())
			{
				return DiskDataConverter<MidNode>::ConvertToDiskData(static_cast<MidNode&>(node));
			}
			else
			{
				return DiskDataConverter<LeafNode>::ConvertToDiskData(static_cast<LeafNode&>(node));
			}
		}

		shared_ptr<Node> ConvertFromDiskData(uint32_t startInFile)
		{
			auto middle = CurrentFile::Read<bool>(startInFile, sizeof(bool));
			auto contentStart = startInFile + sizeof(bool);

			if (middle)
			{
				return DiskDataConverter<LibTreeMid>::ConvertFromDiskData(contentStart);
			}
			else
			{
				return DiskDataConverter<LibTreeLeaf>::ConvertFromDiskData(contentStart);
			}
		}
	};

	template <typename Key, typename Value>
	// BtreeOrder 3 not effect Item size
	constexpr size_t ItemSize = sizeof(typename DiskDataConverter<Elements<Key, Value, 3>>::Item);
	template <typename Key, typename Value>
	constexpr size_t MidElementsItemSize = ItemSize<string, unique_ptr<>>; // TODO should read from converted Mid
	template <typename Key, typename Value>
	constexpr size_t LeafElementsItemSize = ItemSize<string, string>;
	template <typename Key, typename Value>
	constexpr size_t MidConstPartSize = DiskDataConverter<MiddleNode<Key, Value, 3>>::ConstPartSize;
	template <typename Key, typename Value>
	constexpr size_t LeafConstPartSize = DiskDataConverter<LeafNode<Key, Value, 3>>::ConstPartSize;

	constexpr size_t constInMidNode = 4;
	constexpr size_t constInLeafNode = 4;
	template <typename Key, typename Value>
	constexpr size_t BtreeOrder_Mid = (DiskBlockSize - MidConstPartSize<Key, Value>) / MidElementsItemSize<Key, Value>;
	template <typename Key, typename Value>
	constexpr size_t BtreeOrder_Leaf = (DiskBlockSize - LeafConstPartSize<Key, Value>) / LeafElementsItemSize<Key, Value>;
	template <typename Key, typename Value>
	constexpr size_t BtreeOrder = Min(BtreeOrder_Mid<Key, Value>, BtreeOrder_Leaf<Key, Value>);

	template <typename T>
	struct DiskDataConverter<DiskPos<T>>
	{
		using Pos = DiskPos<T>;
		using Index = typename Pos::Index;

		auto ConvertToDiskData(Pos p)
		{
			return DiskDataConverter<Index>::ConvertToDiskData(p._start);
		}

		template <ToPlace Place = ToPlace::Heap>
		auto ConvertFromDiskData(uint32_t startInFile)
		{
			auto i = DiskDataConverter<Index>::ConvertFromDiskData<ToPlace::Stack>(p._start);
			if constexpr (Place == ToPlace::Stack)
			{
				return Pos(i);
			}
			else
			{
				return make_shared<Pos>(i);
			}
		}
	};

	template <typename T>
	struct DiskDataConverter<DiskPtr<T>>
	{
		using Ptr = DiskPtr<T>;

		auto ConvertToDiskData(Ptr& p)
		{
			return DiskDataConverter<decltype(p._pos)>::ConvertToDiskData(p._pos);
		}

		shared_ptr<Ptr> ConvertFromDiskData(uint32_t startInFile)
		{
			return make_shared<Ptr>(
				DiskDataConverter<decltype(p._pos)>::ConvertFromDiskData<ToPlace::Stack>(p._pos));
		}
	};

	template <typename T>
	struct DiskDataConverter<WeakDiskPtr<T>>
	{
		using Ptr = WeakDiskPtr<T>;

		auto ConvertToDiskData(Ptr& p)
		{
			return DiskDataConverter<decltype(p._pos)>::ConvertToDiskData(p._pos);
		}

		shared_ptr<Ptr> ConvertFromDiskData(uint32_t startInFile)
		{
			return make_shared<Ptr>(
				DiskDataConverter<decltype(p._pos)>::ConvertFromDiskData<ToPlace::Stack>(p._pos));
		}
	};
}