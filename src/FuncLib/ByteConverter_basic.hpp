#pragma once
#include <type_traits>
#include <vector>
#include <array>
#include <tuple>
#include <utility>
#include <string>
#include <memory>
#include <cstddef>
#include "../Btree/Elements.hpp"
#include "../Btree/LiteVector.hpp"
#include "../Btree/Basic.hpp"
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
	using ::Collections::Elements;
	using ::Collections::LiteVector;
	using ::Collections::order_int;

	// ����ͳһһ���õ���������

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

	// ������������Ͷ��� Convert ���˵ģ�TypeConverter �� ByteConverter �����ж��п��ܷ���Ӳ�̿ռ�ķ��䣬��������ת���е� string
	// ��Ҫ��֤���� ByteConverter �����в���������ת��

	template <typename T, bool = is_standard_layout_v<T> && is_trivial_v<T>>
	struct ByteConverter;

	template <typename T>
	constexpr size_t ConvertedByteSize = sizeof(ReturnType<decltype(ByteConverter<T>::ConvertToByte)>::Type);

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
		// static_assert(is_class_v<T>, "Only support class type");
		// ���� T ָ���Ǿۺ�����
		static constexpr size_t Size = ConvertedByteSize<T>;

		template <typename Ty, auto... Is>
		static auto CombineEachConvert(Ty const& tup, index_sequence<Is...>)
		{
			auto converter = [&tup]<auto Index>()
			{
				auto& i = get<Index>(tup);
				// Should return array type
				return ByteConverter<decltype(i)>::ConvertToByte(i);
			};

			return (... + converter.template operator()<Is>());
		}

		static auto ConvertToByte(T const& t)
		{
			auto tup = ToTuple(forward<T>(t));// TODO check forward use right?
			return CombineEachConvert(tup, make_index_sequence<tuple_size_v<decltype(tup)>>());
		}

		template <typename Ty, typename UnitConverter, auto... Is>
		static Ty ConsT(UnitConverter converter, index_sequence<Is...>)
		{
			return { converter.template operator ()<Is>()... };
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
			using Tuple = typename ReturnType<decltype(ToTuple<T>)>::Type;
			auto converter = [file = file]<auto Index>()
			{
				constexpr auto start = DiskDataInternalOffset<Index, Tuple>::Offset;
				using SubType = typename tuple_element<Index, Tuple>::type;
				return ByteConverter<SubType>::ConvertFromByte(file, start);
			};

			return ConsT<T>(converter, make_index_sequence<tuple_size_v<Tuple>>());
		}
	};

	// string ���ܱ�Ƕ�׵��ã���Ϊ��С���̶�
	template <>
	struct ByteConverter<string>
	{
		static vector<byte> ConvertToByte(string const& t)
		{
			auto size = t.size();
			auto arr = ByteConverter<decltype(size)>::ConvertToByte(size);
			vector<byte> bytes{ arr.begin(), arr.end() };
			bytes.reserve(size + arr.size());
			byte const *str = reinterpret_cast<byte const *>(t.c_str());
			bytes.insert(bytes.end(), str, str + t.size());
			return bytes;
		}

		static string ConvertFromByte(shared_ptr<File> file, uint32_t startInFile)
		{
			auto size = file->Read<size_t>(startInFile, sizeof(size_t));
			auto contentStart = startInFile + sizeof(size_t);
			auto bytes = file->Read(contentStart, size);
			char* str = reinterpret_cast<char*>(bytes.data());
			return { str, str + bytes.size() };
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
			// TODO count should be Capacity
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
			for (size_t i = 0, offset = sizeof(Capacity); i < count; ++i, offset += unitSize)
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
			return 
			{ 
				ByteConverter<Key>::ConvertFromByte(file, startInFile),
				ByteConverter<Key>::ConvertFromByte(file, startInFile + keySize) 
			};
		}
	};

	template <typename Key, typename Value, order_int Order>
	struct ByteConverter<Elements<Key, Value, Order>, false>
	{
		using ThisType = Elements<Key, Value, Order>;
		static constexpr size_t Size = ByteConverter<LiteVector<pair<Key, Value>, order_int, Order>>::Size;

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

	// template <typename Key, typename Value>
	// // BtreeOrder 3 not effect Item size
	// constexpr size_t ItemSize = sizeof(typename ByteConverter<Elements<Key, Value, 3>>::Item);
	// template <typename Key, typename Value>
	// constexpr size_t MidElementsItemSize = ItemSize<string, unique_ptr<>>; // TODO should read from converted Mid
	// template <typename Key, typename Value>
	// constexpr size_t LeafElementsItemSize = ItemSize<string, string>;
	// template <typename Key, typename Value>
	// constexpr size_t MidConstPartSize = ByteConverter<MiddleNode<Key, Value, 3>>::ConstPartSize;
	// template <typename Key, typename Value>
	// constexpr size_t LeafConstPartSize = ByteConverter<LeafNode<Key, Value, 3>>::ConstPartSize;

	// constexpr size_t constInMidNode = 4;
	// constexpr size_t constInLeafNode = 4;
	// template <typename Key, typename Value>
	// constexpr size_t BtreeOrder_Mid = (DiskBlockSize - MidConstPartSize<Key, Value>) / MidElementsItemSize<Key, Value>;
	// template <typename Key, typename Value>
	// constexpr size_t BtreeOrder_Leaf = (DiskBlockSize - LeafConstPartSize<Key, Value>) / LeafElementsItemSize<Key, Value>;
	// template <typename Key, typename Value>
	// constexpr size_t BtreeOrder = Min(BtreeOrder_Mid<Key, Value>, BtreeOrder_Leaf<Key, Value>);
}