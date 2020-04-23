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
	using ::std::declval;
	using ::std::is_standard_layout_v;
	using ::std::memcpy;
	using ::std::copy;
	using ::std::vector;
	using ::std::array;
	using ::std::pair;
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

	// 这里统一一下用的整数类型

	template <typename T>
	struct ReturnType;

	template <typename R, typename... Args>
	struct ReturnType<R(Args...)>
	{
		using Type = R;
	};

	// TODO test
	template <typename T>
	constexpr size_t ConvertedByteSize = sizeof(ReturnType<decltype(ByteConverter<T>::ConvertToByte)>::Type);

	constexpr size_t Min(size_t one, size_t two)
	{
		return one > two ? one : two;
	}

	// 这里的所有类型都是 Convert 好了的，TypeConverter 和 ByteConverter 过程中都有可能发生硬盘空间的分配，比如类型转换中的 string
	// 需要保证的是 ByteConverter 过程中不发生类型转换

	template <typename T, bool/* = is_standard_layout_v<T> && is_trivial_v<T>*/>
	struct ByteConverter;

	template <typename T>
	struct ByteConverter<T, true>
	{
		static constexpr size_t Size = sizeof(T);

		static array<byte, sizeof(T)> ConvertToByte(T t)
		{
			array<byte, sizeof(T)> mem;
			memcpy(&mem, &t, sizeof(T)); // TODO replace with std::copy?
			return mem;
		}

		static T ConvertFromByte(shared_ptr<File> file, uint32_t startInFile)
		{
			array<byte, sizeof(T)> raw = file->Read<T>(startInFile, sizeof(T));
			T* p = reinterpret_cast<T*>(&raw);
			return *p;
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
		static constexpr size_t Size = ConvertedByteSize<T>;

		template <typename T, auto... Is>
		auto CombineEachConvert(T const& tup, index_sequence<Is...>)
		{
			auto converter = [&tup]<auto Index>()
			{
				auto& i = get<Index>(tup);
				// Should return array type
				return ByteConverter<decltype(i)>::ConvertToByte(i);
			};

			return (... + converter.operator()<Is>());
		}

		static auto ConvertToByte(T const& t)
		{
			auto tup = ToTuple(forward<T>(t));// TODO check forward use right?
			return CombineEachConvert(tup, make_index_sequence<tuple_size_v<decltype(tup)>>());
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
			static constexpr auto CurrentTypeSize = ConvertedByteSize<Head>;
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

	// string 不能被嵌套调用，因为大小不固定
	template <>
	struct ByteConverter<string>
	{
		static vector<byte> ConvertToByte(string const& t)
		{
			auto size = t.size();
			auto arr = ByteConverter<decltype(size)>::ConvertToByte(size);
			vector<byte> bytes{ arr.begin(), arr.end() };
			bytes.reserve(size + arr.size());
			copy(t.begin(), t.end(), bytes.end());
			return bytes;
		}

		static string ConvertFromByte(shared_ptr<File> file, uint32_t startInFile)
		{
			auto size = file->Read<size_t>(startInFile, sizeof(size_t));
			auto contentStart = startInFile + sizeof(size_t);
			auto chars = file->Read(contentStart, size);
			return { chars.begin(), chars.end() };
		}
	};

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
		struct DiskBtree
		{
			decltype(declval<ThisType>()._keyCount) KeyCount;
			decltype(declval<ThisType>()._root) Root;
		};

		static constexpr size_t Size = ByteConverter<DiskBtree>::Size;

		static array<byte, Size> ConvertToByte(ThisType const& t)
		{
			return ByteConverter<DiskBtree>::ConvertToByte({ t._keyCount, t._root });
		}

		static ThisType ConvertFromByte(shared_ptr<File> file, uint32_t startInFile)
		{
			auto t = ByteConverter<DiskBtree>::ConvertFromByte(file, startInFile);
			return { t.KeyCount, move(t.Root) };// TODO set callback inner
		}
	};

	template <typename T, typename size_int, size_int Capacity>
	struct ByteConverter<LiteVector<T, size_int, Capacity>, false>
	{
		using ThisType = LiteVector<T, size_int, Capacity>;
		static constexpr size_t Size = sizeof(Capacity) + ByteConverter<T>::Size * Capacity;

		static array<byte, Size> ConvertToByte(ThisType const& vec)
		{
			constexpr auto unitSize = ByteConverter<T>::Size;
			constexpr auto countSize = sizeof(Capacity);
			array<byte, Size> mem;

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

		static ThisType ConvertFromByte(shared_ptr<File> file, uint32_t startInFile)
		{
			constexpr auto unitSize = ByteConverter<T>::Size;
			auto count = file->Read<decltype(Capacity)>(startInFile, sizeof(Capacity));
			ThisType vec;
			for (auto i = 0, offset = sizeof(Capacity); i < count; ++i, offset += unitSize)
			{
				vec.Add(ByteConverter<T>::ConvertFromByte(file, offset));
			}

			return vec;
		}
	};

	template <typename Key, typename Value>
	struct ByteConverter<pair<Key, Value>, false>
	{
		using ThisType = pair<Key, Value>;
		static constexpr size_t Size = ByteConverter<Key>::Size + ByteConverter<Value>::Size;

		static array<byte, Size> ConvertToByte(ThisType const& t)
		{
			return ByteConverter<Key>::ConvertToByte(t.first)
				+ ByteConverter<Value>::ConvertToByte(t.second);
		}

		static ThisType ConvertFromByte(shared_ptr<File> file, uint32_t startInFile)
		{
			auto keySize = ByteConverter<Key>::Size;
			return { 
				ByteConverter<Key>::ConvertFromByte(file, startInFile),
				ByteConverter<Key>::ConvertFromByte(file, startInFile + keySize) 
			};
		}
	};

	template <typename Key, typename Value, order_int Order>
	struct ByteConverter<Elements<Key, Value, Order>, false>
	{
		using ThisType = Elements<Key, Value, Order>;
		static constexpr size_t Size = LiteVector<pair<Key, Value>, order_int, Order>::Size;

		static array<byte, Size> ConvertToByte(ThisType& t)
		{
			return ByteConverter<LiteVector<pair<Key, Value>, order_int, Order>>::ConvertToByte(t);
		}

		static ThisType ConvertFromByte(shared_ptr<File> file, uint32_t startInFile)
		{
			// Each type should have a constructor of all data member to easy set
			// TODO Check each constructor
			return ByteConverter<LiteVector<pair<Key, Value>, order_int, Order>>::ConvertFromByte(file, startInFile);
		}
	};

	template <typename Key, typename Value, order_int Count>
	struct ByteConverter<MiddleNode<Key, Value, Count, DiskPtr>, false>
	{
		using ThisType = MiddleNode<Key, Value, Count, DiskPtr>;
		static constexpr size_t Size = ByteConverter<decltype(declval<ThisType>()._elements)>::Size;

		static vector<byte> ConvertToByte(ThisType const& t)
		{
			return ByteConverter<decltype(t._elements)>::ConvertToByte(t._elements);
		}

		static ThisType ConvertFromByte(shared_ptr<File> file, uint32_t startInFile)
		{
			using T = decltype(declval<ThisType>()._elements);
			auto elements = ByteConverter<T>::ConvertFromByte(file, startInFile);
			return { move(elements) };// provide LeafNode previous, next and callback inner
		}
	};

	template <typename Key, typename Value, order_int Count>
	struct ByteConverter<LeafNode<Key, Value, Count, DiskPtr>, false>
	{
		using ThisType = LeafNode<Key, Value, Count, DiskPtr>;
		static constexpr size_t Size = ByteConverter<decltype(declval<ThisType>()._elements)>::Size;

		static array<byte, Size> ConvertToByte(ThisType const& t)
		{
			return ByteConverter<decltype(t._elements)>::ConvertToByte(t._elements);
		}

		static ThisType ConvertFromByte(shared_ptr<File> file, uint32_t startInFile)
		{
			using T = decltype(declval<ThisType>()._elements);
			auto elements = ByteConverter<T>::ConvertFromByte(file, startInFile);
			return { move(elements) };
		}
	};

	template <typename Key, typename Value, order_int Count>
	struct ByteConverter<NodeBase<Key, Value, Count, DiskPtr>, false>
	{
		using ThisType = NodeBase<string, string, Count, DiskPtr>;
		using MidNode = MiddleNode<string, string, Count, DiskPtr>;
		using LeafNode = LeafNode<string, string, Count, DiskPtr>;

		static vector<byte> ConvertToByte(ThisType const& node)
		{
			vector<byte> bytes;
			auto middle = node.Middle();
			auto arr = ByteConverter<bool>::ConvertToByte(middle);
			// copy 会自动扩充 vector 里面的 size 吗，应该不是直接裸拷吧
			copy(arr.begin(), arr.end(), bytes.end());

			if (middle)
			{
				auto byteArray = ByteConverter<MidNode>::ConvertToByte(static_cast<MidNode const&>(node));
				copy(byteArray.begin(), byteArray.end(), bytes.end());
				return bytes;
			}
			else
			{
				auto byteArray = ByteConverter<LeafNode>::ConvertToByte(static_cast<LeafNode const&>(node));
				copy(byteArray.begin(), byteArray.end(), bytes.end());
				return bytes;
			}
		}

		shared_ptr<ThisType> ConvertFromByte(shared_ptr<File> file, uint32_t startInFile)
		{
			auto middle = file->Read<bool>(startInFile, sizeof(bool));
			auto contentStart = startInFile + sizeof(middle);

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
}