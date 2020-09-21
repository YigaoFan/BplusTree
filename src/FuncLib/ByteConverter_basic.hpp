#pragma once
#include <type_traits>
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

namespace FuncLib
{
	using ::Basic::All;
	using ::Basic::ReturnType;
	using ::Collections::Elements;
	using ::Collections::LiteVector;
	using ::Collections::order_int;
	using ::std::byte;
	using ::std::copy;
	using ::std::declval;
	using ::std::forward;
	using ::std::get;
	using ::std::index_sequence;
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
	using ::std::is_convertible;
	using namespace Store;

	/// ByteConverter 的工作是将一个类型和 Byte 之间相互转换
	/// TypeConverter 的工作是将一个转换为适合硬盘读取的类型（可以有类似 DiskPtr 这样的读取隔离，而不是一下子读取完）

	template <typename T, bool>
	struct ByteConverter;

	template <typename T>
	struct GetSizeStable
	{
		static constexpr bool Result = ByteConverter<T>::SizeStable;
	};
	
	template <typename T>
	struct ByteConverter<T, true>
	{
		static constexpr bool SizeStable = true;

		static void WriteDown(T const& t, shared_ptr<FileWriter> writer)
		{
			char const* start = reinterpret_cast<char const*>(&t);
			writer->Write(start, sizeof(T));
		}

		static T ReadOut(shared_ptr<FileReader> reader)
		{
			auto bytes = reader->Read<sizeof(T)>();
			T* p = reinterpret_cast<T*>(&bytes[0]);
			return *p;
		}
	};

	template <typename T>
	struct ByteConverter<T, false>
	{
		using Tuple = typename ReturnType<decltype(ToTuple<T>)>::Type;
		static constexpr bool SizeStable = All<GetSizeStable, Tuple>::Result;

		template <typename Ty, auto... Is>
		static void WriteEach(shared_ptr<FileWriter> writer, Ty const& tup, index_sequence<Is...>)
		{
			auto converter = [&]<auto Index>()
			{
				auto& i = get<Index>(tup);
				ByteConverter<remove_cvref_t<decltype(i)>>::WriteDown(i, writer);
			};

			(converter.template operator()<Is>(), ...);
		}

		static void WriteDown(T const& t, shared_ptr<FileWriter> writer)
		{
			auto tup = ToTuple(forward<decltype(t)>(t));
			WriteEach(writer, tup, make_index_sequence<tuple_size_v<decltype(tup)>>());
		}

		template <typename Ty, typename UnitConverter, auto... Is>
		static Ty ConsT(UnitConverter converter, index_sequence<Is...>)
		{
			return { converter.template operator ()<Is>()... };
		}

		static T ReadOut(shared_ptr<FileReader> reader)
		{
			auto converter = [&]<auto Index>()
			{
				using SubType = typename tuple_element<Index, Tuple>::type;
				return ByteConverter<SubType>::ReadOut(reader);
			};

			return ConsT<T>(converter, make_index_sequence<tuple_size_v<Tuple>>());
		}
	};

	template <typename T>
	concept StrConvertible = is_convertible<T, string>::value;

	template <typename T>
	requires StrConvertible<T>
	struct ByteConverter<T, false>
	{
		static constexpr bool SizeStable = false;

		static void WriteDown(string const& t, shared_ptr<FileWriter> writer)
		{
			auto n = t.size();
			ByteConverter<decltype(n)>::WriteDown(n, writer);
			writer->Write(t.c_str(), t.size());
		}

		static string ReadOut(shared_ptr<FileReader> reader)
		{
			auto charCount = ByteConverter<size_t>::ReadOut(reader);
			auto bytes = reader->Read(charCount);
			char* str = reinterpret_cast<char*>(bytes.data());
			return { str, str + charCount };
		}
	};

	template <typename T, typename size_int, size_int Capacity>
	struct ByteConverter<LiteVector<T, size_int, Capacity>, false>
	{
		static constexpr bool SizeStable = GetSizeStable<T>::Result;
		using ThisType = LiteVector<T, size_int, Capacity>;

		static void WriteDown(ThisType const& vec, shared_ptr<FileWriter> writer)
		{
			// Count
			auto n = vec.Count();
			ByteConverter<size_int>::WriteDown(n, writer);
			// Items
			auto start = writer->CurrentPos();
			for (auto& t : vec)
			{
				ByteConverter<T>::WriteDown(t, writer);
			}

			if constexpr (SizeStable)
			{
				auto unitSize = (writer->CurrentPos() - start) / n;
				writer->WriteBlank(unitSize * (Capacity - n));
			}
		}

		static ThisType ReadOut(shared_ptr<FileReader> reader)
		{
			auto n = ByteConverter<size_int>::ReadOut(reader);
			ThisType vec;
			for (size_t i = 0; i < n; ++i)
			{
				vec.Add(ByteConverter<T>::ReadOut(reader));
			}

			return vec;
		}
	};

	template <typename Key, typename Value>
	struct ByteConverter<pair<Key, Value>, false>
	{
		using ThisType = pair<Key, Value>;
		static constexpr bool SizeStable = All<GetSizeStable, Key, Value>::Result;

		static constexpr size_t Size = ByteConverter<Key>::Size + ByteConverter<Value>::Size;

		static void WriteDown(ThisType const& t, shared_ptr<FileWriter> writer)
		{
			ByteConverter<Key>::WriteDown(t.first, writer);
			ByteConverter<Value>::WriteDown(t.second, writer);
		}

		static ThisType ReadOut(shared_ptr<FileReader> reader)
		{
			auto k = ByteConverter<Key>::ReadOut(reader);
			auto v = ByteConverter<Value>::ReadOut(reader);
			return { move(k), move(v) };
		}
	};

	template <typename Key, typename Value, order_int Order, typename LessThan>
	struct ByteConverter<Elements<Key, Value, Order, LessThan>, false>
	{
		using ThisType = Elements<Key, Value, Order, LessThan>;
		using BaseType = LiteVector<pair<Key, Value>, order_int, Order>;
		static constexpr bool SizeStable = All<GetSizeStable, BaseType>::Result;

		static void WriteDown(ThisType const& t, shared_ptr<FileWriter> writer)
		{
			ByteConverter<BaseType>::WriteDown(t, writer);
		}

		static ThisType ReadOut(shared_ptr<FileReader> reader)
		{
			// Each type should have a constructor of all data member to easy set
			// Like Elements have a constructor which has LiteVector as arg
			return ByteConverter<BaseType>::ReadOut(reader);
		}
	};
}