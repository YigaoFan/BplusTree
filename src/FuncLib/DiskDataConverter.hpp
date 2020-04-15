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
#include "DiskAllocator.hpp"
#include "DiskPtr.hpp"
#include "CurrentFile.hpp"
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
		return sizeof(ReturnType<decltype(DiskDataConverter<T>::ConvertToDiskData)>::Type);
	}

	constexpr size_t Min(size_t one, size_t two)
	{
		return one > two ? one : two;
	}

	template <typename T, bool = is_standard_layout_v<T>&& is_trivial_v<T>>
	struct DiskDataConverter;

	template <typename T>
	struct DiskDataConverter<T, true>
	{
		static array<byte, sizeof(T)> ConvertToDiskData(T t)
		{
			array<byte, sizeof(T)> mem;
			memcpy(&mem, &t, sizeof(T)); // TODO replace with std::copy?
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
	struct DiskDataConverter<T, false>
	{
		static_assert(is_class_v<T>, "Only support class type when not specialize");

		template <typename T, auto... Is>
		auto CombineEachConvert(T&& tup, index_sequence<Is...>)
		{
			auto converter = [&tup]<auto Index>()
			{
				auto& i = get<Index>(tup);
				// Should return array type
				return DiskDataConverter<decltype(i)>::ConvertToDiskData(i);
			};

			return (... + converter.operator ()<Is>());
		}

		static auto ConvertToDiskData(T&& t)
		{
			auto tup = ToTuple(forward<T>(t));// TODO check forward use right?
			// TODO how to forward or move tup
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
			static constexpr auto CurrentTypeSize = CalNeedDiskSize<Head>();
			static constexpr auto Offset = CurrentTypeSize + NextUnit::Offset;
		};

		static T ConvertFromDiskData(uint32_t startInFile)
		{
			using Tuple = ReturnType<ToTuple<T>>::Type;
			auto converter = []<auto Index>()
			{
				constexpr auto start = DiskDataInternalOffset<Index, Tuple>::Offset;
				using T = tuple_element<Index, Tuple>::type;
				return DiskDataConverter<T>::ConvertFromDiskData(start);
			};

			return ConsT<T>(converter, make_index_sequence<tuple_size_v<decltype(tup)>>());
		}
	};

	template <>
	struct DiskDataConverter<string>
	{
		// TODO return value could be iterated
		template <SizeDemand Demand = SizeDemand::Fixed>
		auto ConvertToDiskData(string const& t)
		{
			if constexpr (Demand == SizeDemand::Dynamic)
			{
				return vector<byte>(t.begin(), t.end());
			}
			else
			{
				auto chars = ConvertToDiskData<SizeDemand::Dynamic>(t);
				// use chars to cons a DiskPtr, return DiskPtr::ConvertToDiskDataS
			}
		}

		// TODO 这里可能也要区分是从指针 ConvertFrom 的，还是直接读的是字符串
		// 两者读取会稍有不同
		shared_ptr<string> ConvertFromDiskData(uint32_t startInFile)
		{
			auto size = CurrentFile::Read<uint32_t>(startInFile, sizeof(uint32_t));
			auto contentStart = startInFile + sizeof(uint32_t);
			auto chars = CurrentFile::Read(contentStart, size);
			return make_shared<string>(chars.begin(), chars.end());
		}
	};

	//using LibTree = Btree<3, string, string>;
	//using LibTreeLeaf = LeafNode<string, string, 3>;
	//using LibTreeLeafEle = Elements<string, string, 3>;
	//using LibTreeMid = MiddleNode<string, string, 3>;
	//using LibTreeMidEle = Elements<string, string, 3>;

	template <typename Key, typename Value, order_int Order>
	struct DiskDataConverter<Btree<Order, Key, Value>, false>// TODO why here need false, string not need, see error info
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
	struct DiskDataConverter<LiteVector<T, size_int, Capacity>, false>
	{
		using ThisType = LiteVector<T, size_int, Capacity>;

		static auto ConvertToDiskData(ThisType const& vec)
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
				auto s = &DiskDataConverter<T>::ConvertToDiskData(vec[i]);
				auto d = &mem[i * unitSize + countSize];
				memcpy(d, s, unitSize);
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
	struct DiskDataConverter<Elements<Key, Value, Order>, false>
	{
		using ThisType = Elements<Key, Value, Order>;

		static auto ConvertToDiskData(ThisType& t)
		{
			struct Item
			{
				Key Key;
				Value Value;
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

	template <typename Key, typename Value, order_int Count>
	struct Node
	{
		bool Middle;
		DiskPtr<NodeBase<Key, Value, Count>> UpNodePtr;
		DiskPtr<Btree<Count, Key, Value>> BtreePtr;
		// TODO
		// 需要把三个 Node 相关的类的模板接口和实现分开声明，才能在实现中相互引用
	};

	template <typename Key, typename Value, auto Count>
	struct DiskDataConverter<MiddleNode<Key, Value, Count>, false>
	{
		using ThisType = MiddleNode<Key, Value, Count>;

		struct DiskMid
		{
			Node<Key, Value, Count> Base;
			void* UpPtr;
			Elements<Key, Value, Count> Elements; // 注意这里的 Key 可能是引用，Value 是指针
		};

		static auto ConvertToDiskData(ThisType& t, DiskPtr<ThisType> upNode)
		{
			return DiskDataConverter<DiskMid>::ConvertToDiskData({ true, nullptr, });
		}

		static shared_ptr<ThisType> ConvertFromDiskData(uint32_t startInFile)
		{

		}
	};

	template <typename Key, typename Value, auto Count>
	struct DiskDataConverter<LeafNode<Key, Value, Count>, false>
	{
		using ThisType = LeafNode<Key, Value, Count>;

		struct DiskLeaf
		{
			Node<Key, Value, Count> Base;
			void* PreLeaf;// 这个还有被保存的顺序要求，可不可以实现某种递归，一个一个向底下触发，这应该就要求有某种缓存
			void* NxtLeaf;
			Elements<Key, Value, Count> Elements;
		};

		using Converted = DiskLeaf;
		// preNode and nextNode should be stored at DiskBtreeConverter
		// 所以这个类型的 ConvertToDiskData 就不符合通用的接口了——只一个 ThisType 对象参数
		static auto ConvertToDiskData(ThisType& t, DiskPtr<ThisType> preNode, DiskPtr<ThisType> nextNode)
		{
			auto middle = false;
			CurrentFile::Write<bool>();

		}

		static shared_ptr<ThisType> ConvertFromDiskData(uint32_t startInFile)
		{
		}
	};

	template <typename Key, typename Value, order_int Count>
	struct DiskDataConverter<NodeBase<Key, Value, Count>, false>
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
				return DiskDataConverter<MidNode>::ConvertFromDiskData(contentStart);
			}
			else
			{
				return DiskDataConverter<LeafNode>::ConvertFromDiskData(contentStart);
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
	struct DiskDataConverter<DiskPos<T>, false>
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
	struct DiskDataConverter<DiskPtr<T>, false>
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
	struct DiskDataConverter<WeakDiskPtr<T>, false>
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