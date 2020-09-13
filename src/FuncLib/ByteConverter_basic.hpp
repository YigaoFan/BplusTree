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
#include "../Basic/TypeTrait.hpp"
#include "./Store/FileReader.hpp"
#include "./Store/FileWriter.hpp"
#include "StructToTuple.hpp"

// namespace std
// {
// 	using ::std::array;
// 	using ::std::index_sequence;
// 	using ::std::make_index_sequence;

// 	template <typename T, auto N1, auto... Is1, auto N2, auto... Is2>
// 	array<T, N1 + N2> AddArray(array<T, N1> a1, index_sequence<Is1...>, array<T, N2> a2, index_sequence<Is2...>)
// 	{
// 		return
// 		{
// 			move(a1[Is1])...,
// 			move(a2[Is2])...,
// 		};
// 	}

// 	// operator+ cannot be found if it's in FuncLib namespace, but in std OK
// 	// maybe related to Argument Dependent Lookup
// 	template <typename T, auto N1, auto N2>
// 	array<T, N1 + N2> operator+ (array<T, N1> a1, array<T, N2> a2)
// 	{
// 		auto is1 = make_index_sequence<N1>();
// 		auto is2 = make_index_sequence<N2>();
// 		return AddArray(move(a1), is1, move(a2), is2);
// 	}
// }

namespace FuncLib
{
	using ::Basic::ReturnType;
	using ::Collections::Elements;
	using ::Collections::LiteVector;
	using ::Collections::order_int;
	using ::std::array;
	using ::std::byte;
	using ::std::copy;
	using ::std::declval;
	using ::std::forward;
	using ::std::get;
	using ::std::index_sequence;
	using ::std::is_class_v;
	using ::std::make_index_sequence;
	using ::std::make_shared;
	using ::std::memcpy;
	using ::std::move;
	using ::std::pair;
	using ::std::remove_cvref_t;
	using ::std::shared_ptr;
	using ::std::size;
	using ::std::size_t;
	using ::std::string;
	using ::std::tuple;
	using ::std::tuple_element;
	using ::std::tuple_size_v;
	using ::std::unique_ptr;
	using ::std::vector;
	using namespace Store;

	/// ByteConverter 的工作是将一个类型和 Byte 之间相互转换
	/// TypeConverter 的工作是将一个转换为适合硬盘读取的类型（可以有类似 DiskPtr 这样的读取隔离，而不是一下子读取完）

	template <typename T, bool>
	struct ByteConverter;

	template <typename T>
	constexpr size_t ConvertedByteSize = sizeof(typename ReturnType<decltype(ByteConverter<T>::ConvertToByte)>::Type);

	template <typename T>
	struct ByteConverter<T, true>
	{
		static constexpr size_t Size = sizeof(T);

		static void ConvertToByte(T const& t, shared_ptr<FileWriter> writer)
		{
			// 以这个函数作为示范，去修改其他的 ConvertToByte
			array<byte, sizeof(T)> mem;
			memcpy(&mem, &t, sizeof(T)); // TODO replace with std::copy?
			writer->Write(mem);
		}

		static T ConvertFromByte(shared_ptr<FileReader> reader)
		{
			auto bytes = reader->Read<sizeof(T)>();
			T* p = reinterpret_cast<T*>(&bytes[0]);
			return *p;
		}
	};

	template <typename T>
	struct ByteConverter<T, false>
	{
		template <typename Ty, auto... Is>
		static void WriteEach(shared_ptr<FileWriter> writer, Ty const& tup, index_sequence<Is...>)
		{
			auto converter = [&tup, &writer]<auto Index>()
			{
				auto& i = get<Index>(tup);
				ByteConverter<remove_cvref_t<decltype(i)>>::ConvertToByte(i, writer);
			};

			(... + converter.template operator()<Is>());
		}

		static void ConvertToByte(T const& t, shared_ptr<FileWriter> writer)
		{
			auto tup = ToTuple(forward<decltype(t)>(t));
			WriteEach(writer, tup, make_index_sequence<tuple_size_v<decltype(tup)>>());
		}

		template <typename Ty, typename UnitConverter, auto... Is>
		static Ty ConsT(UnitConverter converter, index_sequence<Is...>)
		{
			return { converter.template operator ()<Is>()... };
		}

		static T ConvertFromByte(shared_ptr<FileReader> reader)
		{
			// ConvertToByte use T const&, here should use ToTuple<T const&>? either can pass compile
			using Tuple = typename ReturnType<decltype(ToTuple<T>)>::Type;
			auto converter = [=reader]<auto Index>()
			{
				using SubType = typename tuple_element<Index, Tuple>::type;
				return ByteConverter<SubType>::ConvertFromByte(reader);
			};

			return ConsT<T>(converter, make_index_sequence<tuple_size_v<Tuple>>());
		}
	};

	template <>
	struct ByteConverter<string>
	{
		static void ConvertToByte(string const& t, shared_ptr<FileWriter> writer)
		{
			auto n = t.size();
			ByteConverter<decltype(n)>::ConvertToByte(n, writer);
			writer->Write(t.begin(), t.end());
		}

		static string ConvertFromByte(shared_ptr<FileReader> reader)
		{
			auto charCount = ByteConverter<size_t>::ConvertFromByte(reader);
			auto bytes = reader->Read(charCount);
			char* str = reinterpret_cast<char*>(bytes.data());
			return { str, str + charCount };
		}
	};

	template <typename T, typename size_int, size_int Capacity>
	struct ByteConverter<LiteVector<T, size_int, Capacity>, false>
	{
		using ThisType = LiteVector<T, size_int, Capacity>;

		static void ConvertToByte(ThisType const& vec, shared_ptr<FileWriter> writer)
		{
			// Count
			auto n = vec.Count();
			ByteConverter<size_int>::ConvertToByte(n, writer);
			// Items
			for (auto& t : vec)
			{
				ByteConverter<T>::ConvertToByte(vec[i], writer);
			}
		}

		static ThisType ConvertFromByte(shared_ptr<FileReader> reader)
		{
			auto n = ByteConverter<size_int>::ConvertFromByte(reader);
			ThisType vec;
			for (size_t i = 0; i < n; ++i)
			{
				vec.Add(ByteConverter<T>::ConvertFromByte(reader));
			}

			return vec;
		}
	};

	template <typename Key, typename Value>
	struct ByteConverter<pair<Key, Value>, false>
	{
		using ThisType = pair<Key, Value>;
		static constexpr size_t Size = ByteConverter<Key>::Size + ByteConverter<Value>::Size;

		static void ConvertToByte(ThisType const& t, shared_ptr<FileWriter> writer)
		{
			ByteConverter<Key>::ConvertToByte(t.first, writer);
			ByteConverter<Value>::ConvertToByte(t.second, writer);
		}

		static ThisType ConvertFromByte(shared_ptr<FileReader> reader)
		{
			auto k = ByteConverter<Key>::ConvertFromByte(reader);
			auto v = ByteConverter<Value>::ConvertFromByte(reader);
			return { move(k), move(v) };
		}
	};

	template <typename Key, typename Value, order_int Order, typename LessThan>
	struct ByteConverter<Elements<Key, Value, Order, LessThan>, false>
	{
		using ThisType = Elements<Key, Value, Order, LessThan>;

		static void ConvertToByte(ThisType const& t, shared_ptr<FileWriter> writer)
		{
			return ByteConverter<LiteVector<pair<Key, Value>, order_int, Order>>::ConvertToByte(t, writer);
		}

		static ThisType ConvertFromByte(shared_ptr<FileReader> reader)
		{
			// Each type should have a constructor of all data member to easy set
			// Like Elements have a constructor which has LiteVector as arg
			return ByteConverter<LiteVector<pair<Key, Value>, order_int, Order>>::ConvertFromByte(reader);
		}
	};
}