#pragma once
#include <type_traits>
#include <vector>
#include <array>
#include <tuple>
#include <utility>
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
#include "DiskPtr.hpp"
#include "File.hpp"
#include "StructToTuple.hpp"

namespace FuncLib
{
	using ::std::is_trivial_v;
	using ::std::is_standard_layout_v;
	using ::std::memcpy;
	using ::std::vector;
	using ::std::array;
	using ::std::string;
	using ::std::shared_ptr;
	using ::std::unique_ptr;
	using ::std::make_shared;
	using ::std::byte;
	using ::std::size_t;
	using ::std::is_class_v;
	using ::std::forward;
	using ::std::index_sequence;
	using ::std::make_index_sequence;
	using ::std::tuple;
	using ::std::tuple_size_v;
	using ::std::tuple_element;
	using ::std::size;
	using ::std::get;
	using ::std::move;
	using ::Collections::Btree;
	using ::Collections::NodeBase;
	using ::Collections::LeafNode;
	using ::Collections::MiddleNode;
	using ::Collections::Elements;
	using ::Collections::LiteVector;
	using ::Collections::order_int;
	using ::Collections::key_int;
	using ::Collections::LessThan;
	
	enum class ToPlace
	{
		Stack,
		Heap,
	};

	enum class SizeDemand
	{
		Fixed,
		Dynamic,
	};

	template <typename T>
	struct ReturnType;

	template <typename R, typename... Args>
	struct ReturnType<R(Args...)>
	{
		using Type = R;
	};

	// TODO test
	template <typename T>
	constexpr size_t CalNeedDiskSize()
	{
		return sizeof(ReturnType<decltype(ByteConverter<T>::ConvertToByte)>::Type);
	}

	constexpr size_t Min(size_t one, size_t two)
	{
		return one > two ? one : two;
	}

	// 这里的所有类型都是 Convert 好了的，TypeConverter 和 ByteConverter 过程中都有可能发生硬盘空间的分配，比如类型转换中的 string
	// 需要保证的是 ByteConverter 过程中不发生类型转换

	template <typename T, bool = is_standard_layout_v<T> && is_trivial_v<T>>
	struct ByteConverter;

	template <typename T>
	struct ByteConverter<T, true>
	{
		static array<byte, sizeof(T)> ConvertToByte(T t)
		{
			array<byte, sizeof(T)> mem;
			memcpy(&mem, &t, sizeof(T)); // TODO replace with std::copy?
			return mem;
		}

		template <ToPlace Place = ToPlace::Heap>
		static auto ConvertFromByte(shared_ptr<File> file, uint32_t startInFile)
		{
			array<byte, sizeof(T)> raw = file->Read<T>(startInFile, sizeof(T));
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

	template <typename T, auto N1, auto... Is1, auto N2, auto... Is2>
	array<T, N1 + N2> AddArray(array<T, N1> a1, index_sequence<Is1...>, array<T, N2> a2, index_sequence<Is2...>)
	{
		return { move(a1[Is1])..., move(a2[Is2])..., };
	}

	template <typename T, auto N1, auto N2>
	array<T, N1 + N2> operator+ (array<T, N1> a1, array<T, N2> a2)
	{
		auto is1 = make_index_sequence<N1>();
		auto is2 = make_index_sequence<N2>();
		return AddArray(move(a1), is1, move(a2), is2);
	}

	template <typename T>
	struct ByteConverter<T, false>
	{
		static_assert(is_class_v<T>, "Only support class type");
		// 这里 T 指的是聚合类吗？

		template <typename T, auto... Is>
		auto CombineEachConvert(T&& tup, index_sequence<Is...>)
		{
			auto converter = [&tup]<auto Index>()
			{
				auto& i = get<Index>(tup);
				// Should return array type
				return ByteConverter<decltype(i)>::ConvertToByte(i);
			};

			return (... + converter.operator ()<Is>());
		}

		static auto ConvertToByte(T&& t)
		{
			auto tup = ToTuple(forward<T>(t));// TODO check forward use right?
			// TODO how to forward or move tup
			return CombineEachConvert(move(tup), make_index_sequence<tuple_size_v<decltype(tup)>>());
		}

		template <typename T, typename UnitConverter, auto... Is>
		static T ConsT(UnitConverter converter, index_sequence<Is...>)
		{
			return { converter.operator ()<Is>()... };
		}

		template <auto Index, typename Tuple>
		struct DiskDataInternalOffset;

		template <typename Head, typename... Tail>
		struct DiskDataInternalOffset<0, tuple<Head, Tail...>>
		{
			static constexpr auto Offset = 0;
		};

		template <auto Index, typename Head, typename... Tail>
		struct DiskDataInternalOffset<Index, tuple<Head, Tail...>>
		{
			using NextUnit = DiskDataInternalOffset<Index - 1, tuple<Tail...>>;
			static constexpr auto CurrentTypeSize = CalNeedDiskSize<Head>();
			static constexpr auto Offset = CurrentTypeSize + NextUnit::Offset;
		};

		static T ConvertFromByte(shared_ptr<File> file, uint32_t startInFile)
		{
			using Tuple = ReturnType<ToTuple<T>>::Type;
			auto converter = [file = file]<auto Index>()
			{
				constexpr auto start = DiskDataInternalOffset<Index, Tuple>::Offset;
				using T = tuple_element<Index, Tuple>::type;
				return ByteConverter<T>::ConvertFromByte(file, start);
			};

			return ConsT<T>(converter, make_index_sequence<tuple_size_v<decltype(tup)>>());
		}
	};

	// 由最上面的话，这里没有 string
	//template <>
	//struct ByteConverter<string>
	//{
	//	template <SizeDemand Demand = SizeDemand::Fixed>
	//	auto ConvertToByte(string const& t)
	//	{
	//		if constexpr (Demand == SizeDemand::Dynamic)
	//		{
	//			return vector<byte>(t.begin(), t.end());
	//		}
	//		else
	//		{
	//			auto chars = ConvertToByte<SizeDemand::Dynamic>(t);
	//			// use chars to cons a DiskPtr, return DiskPtr::ConvertToByteS
	//		}
	//	}

	//	// TODO 这里可能也要区分是从指针 ConvertFrom 的，还是直接读的是字符串
	//	// 两者读取会稍有不同
	//	shared_ptr<string> ConvertFromByte(shared_ptr<File> file, uint32_t startInFile)
	//	{
	//		auto size = file->Read<uint32_t>(startInFile, sizeof(uint32_t));
	//		auto contentStart = startInFile + sizeof(uint32_t);
	//		auto chars = file->Read(contentStart, size);
	//		return make_shared<string>(chars.begin(), chars.end());
	//	}
	//};

	//using LibTree = Btree<3, string, string>;
	//using LibTreeLeaf = LeafNode<string, string, 3>;
	//using LibTreeLeafEle = Elements<string, string, 3>;
	//using LibTreeMid = MiddleNode<string, string, 3>;
	//using LibTreeMidEle = Elements<string, string, 3>;



	// Btree 要对两个类 friend，ByteConverter 和 TypeConverter。其它类类似
	template <typename Key, typename Value, order_int Order>
	struct ByteConverter<Btree<Order, Key, Value, DiskPtr>, false>
	{
		using ThisType = Btree<Order, Key, Value, DiskPtr>;
		struct BtreeOnDisk
		{
			key_int KeyCount;
			DiskPtr<NodeBase<Key, Value, Order, DiskPtr>> Root;
		};

		static auto ConvertToByte(ThisType const& t)
		{
			return ByteConverter<BtreeOnDisk>::ConvertToByte({ t._keyCount, t._root });
		}

		static shared_ptr<ThisType> ConvertFromByte(shared_ptr<File> file, uint32_t startInFile, shared_ptr<LessThan<Key>> lessThanPtr)
		{
			auto p = ByteConverter<BtreeOnDisk>::ConvertFromByte(startInFile);
			return make_shared<ThisType>(move(p.Root), p.KeyCount, lessThanPtr);
		}
	};

	// T is the type which could directly copy into disk
	template <typename T, typename size_int, size_int Capacity>
	struct ByteConverter<LiteVector<T, size_int, Capacity>, false>
	{
		using ThisType = LiteVector<T, size_int, Capacity>;

		static auto ConvertToByte(ThisType const& vec)
		{
			constexpr auto unitSize = CalNeedDiskSize<T>();
			constexpr auto countSize = sizeof(Capacity);
			array<byte, countSize + unitSize * Capacity> mem;

			// Count
			auto c = vec.Count();
			memcpy(&mem, &c, countSize);
			// Items
			for (auto i = 0; i < vec.Count(); ++i)
			{
				auto s = &ByteConverter<T>::ConvertToByte(vec[i]);
				auto d = &mem[i * unitSize + countSize];
				memcpy(d, s, unitSize);
			}

			return mem;
		}

		template <ToPlace Place = ToPlace::Heap>
		static auto ConvertFromByte(shared_ptr<File> file, uint32_t startInFile)
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
	struct ByteConverter<Elements<Key, Value, Order>, false>
	{
		using ThisType = Elements<Key, Value, Order>;
		
		static auto ConvertToByte(ThisType& t)
		{
			struct Item
			{
				Convert<Key>::Type Key;
				Convert<Value>::Type Value;
			};
			LiteVector<Item, order_int, Order> vec;

			for (auto& i : t)
			{
				vec.Add({i.first, i.second});
			}

			return ByteConverter<Item>::ConvertToByte(vec);
		}

		static shared_ptr<ThisType> ConvertFromByte(shared_ptr<File> file, uint32_t startInFile, shared_ptr<LessThan<Key>> lessThanPtr)
		{
			// Each type should have a constructor of all data member to easy set

			// Read LiteVector
			// And set LessThan
		}
	};

	template <typename Key, typename Value, order_int Count>
	struct DiskNode
	{
		bool Middle;
		Elements<Key, Value, Count> Elements;
	};

	template <typename Key, typename Value, auto Count>
	struct ByteConverter<MiddleNode<Key, Value, Count, DiskPtr>, false>
	{
		using ThisType = MiddleNode<Key, Value, Count>;

		static vector<byte> ConvertToByte(ThisType const& t)
		{
			DiskNode<Key, Value, Count> node{ true, t._elements };
			return ByteConverter<decltype(node)>::ConvertToByte(node);
		}

		static shared_ptr<ThisType> ConvertFromByte(shared_ptr<File> file, uint32_t startInFile)
		{
			// provide LeafNode previous, next and callback

		}
	};

	template <typename Key, typename Value, auto Count>
	struct ByteConverter<LeafNode<Key, Value, Count, DiskPtr>, false>
	{
		using ThisType = LeafNode<Key, Value, Count, DiskPtr>;

		static auto ConvertToByte(ThisType const& t)
		{
			DiskNode<Key, Value, Count> node{ false, t._elements };
			return ByteConverter<decltype(node)>::ConvertToByte(node);
		}

		static shared_ptr<ThisType> ConvertFromByte(shared_ptr<File> file, uint32_t startInFile)
		{
			// Jmp Middle Byte
		}
	};

	template <typename Key, typename Value, order_int Count>
	struct ByteConverter<NodeBase<Key, Value, Count, DiskPtr>, false>
	{
		using ThisType = NodeBase<string, string, Count, DiskPtr>;
		using MidNode = MiddleNode<string, string, Count, DiskPtr>;
		using LeafNode = LeafNode<string, string, Count, DiskPtr>;

		static vector<byte> ConvertToByte(ThisType const* node)
		{
			if (node.Middle())
			{
				auto byteArray = ByteConverter<MidNode>::ConvertToByte(static_cast<MidNode const&>(*node));
				return { byteArray.begin(), byteArray.end() };
			}
			else
			{
				auto byteArray = ByteConverter<LeafNode>::ConvertToByte(static_cast<LeafNode const&>(*node));
				return { byteArray.begin(), byteArray.end() };
			}
		}

		shared_ptr<ThisType> ConvertFromByte(shared_ptr<File> file, uint32_t startInFile)
		{
			auto middle = CurrentFile::Read<bool>(startInFile, sizeof(bool));
			auto contentStart = startInFile + sizeof(bool);

			if (middle)
			{
				return ByteConverter<MidNode>::ConvertFromByte(file, contentStart);
			}
			else
			{
				return ByteConverter<LeafNode>::ConvertFromByte(file, contentStart);
			}
		}
	};

	template <typename Key, typename Value>
	// BtreeOrder 3 not effect Item size
	constexpr size_t ItemSize = sizeof(typename ByteConverter<Elements<Key, Value, 3>>::Item);
	template <typename Key, typename Value>
	constexpr size_t MidElementsItemSize = ItemSize<string, unique_ptr<>>; // TODO should read from converted Mid
	template <typename Key, typename Value>
	constexpr size_t LeafElementsItemSize = ItemSize<string, string>;
	template <typename Key, typename Value>
	constexpr size_t MidConstPartSize = ByteConverter<MiddleNode<Key, Value, 3>>::ConstPartSize;
	template <typename Key, typename Value>
	constexpr size_t LeafConstPartSize = ByteConverter<LeafNode<Key, Value, 3>>::ConstPartSize;

	constexpr size_t constInMidNode = 4;
	constexpr size_t constInLeafNode = 4;
	template <typename Key, typename Value>
	constexpr size_t BtreeOrder_Mid = (DiskBlockSize - MidConstPartSize<Key, Value>) / MidElementsItemSize<Key, Value>;
	template <typename Key, typename Value>
	constexpr size_t BtreeOrder_Leaf = (DiskBlockSize - LeafConstPartSize<Key, Value>) / LeafElementsItemSize<Key, Value>;
	template <typename Key, typename Value>
	constexpr size_t BtreeOrder = Min(BtreeOrder_Mid<Key, Value>, BtreeOrder_Leaf<Key, Value>);

	template <typename T>
	struct ByteConverter<DiskPos<T>, false>
	{
		using ThisType = DiskPos<T>;
		using Index = typename ThisType::Index;

		auto ConvertToByte(ThisType const& p)
		{
			return ByteConverter<Index>::ConvertToByte(p._start);
		}

		template <ToPlace Place = ToPlace::Heap>
		auto ConvertFromByte(shared_ptr<File> file, uint32_t startInFile)
		{
			auto i = ByteConverter<Index>::ConvertFromByte<ToPlace::Stack>(file, p._start);
			if constexpr (Place == ToPlace::Stack)
			{
				return ThisType(i);
			}
			else
			{
				return make_shared<ThisType>(i);
			}
		}
	};

	template <typename T>
	struct ByteConverter<DiskPtr<T>, false>
	{
		using ThisType = DiskPtr<T>;

		auto ConvertToByte(ThisType const& p)
		{
			return ByteConverter<decltype(p._pos)>::ConvertToByte(p._pos);
		}

		shared_ptr<ThisType> ConvertFromByte(shared_ptr<File> file, uint32_t startInFile)
		{
			return make_shared<ThisType>(
				ByteConverter<decltype(p._pos)>::ConvertFromByte<ToPlace::Stack>(file, p._pos));
		}
	};

	template <typename T>
	struct ByteConverter<WeakDiskPtr<T>, false>
	{
		using ThisType = WeakDiskPtr<T>;

		auto ConvertToByte(ThisType const& p)
		{
			return ByteConverter<decltype(p._pos)>::ConvertToByte(p._pos);
		}

		// TODO 下面这种操作的接口可能会改，因为本来就是来自 Pos 的操作吗
		shared_ptr<ThisType> ConvertFromByte(shared_ptr<File> file, uint32_t startInFile)
		{
			return make_shared<ThisType>(
				ByteConverter<decltype(p._pos)>::ConvertFromByte<ToPlace::Stack>(file, p._pos));
		}
	};
}