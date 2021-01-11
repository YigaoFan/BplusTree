#pragma once
#include <type_traits>
#include <tuple>
#include <utility>
#include <string>
#include <memory>
#include <cstddef>
#include <map>
#include <vector>
#include <cstdint>
#include "../../Btree/Elements.hpp"
#include "../../Btree/LiteVector.hpp"
#include "../../Btree/Basic.hpp"
#include "../../Basic/TypeTrait.hpp"
#include "../Store/FileReader.hpp"
#include "IWriterIReaderConcept.hpp"
#include "StructToTuple.hpp"

namespace FuncLib::Persistence
{
	using ::Basic::All;
	using ::Basic::ReturnType;
	using ::Basic::Sum;
	using ::Collections::Elements;
	using ::Collections::LiteVector;
	using ::Collections::order_int;
	using ::std::byte;
	using ::std::declval;
	using ::std::forward;
	using ::std::get;
	using ::std::index_sequence;
	using ::std::is_convertible;
	using ::std::make_index_sequence;
	using ::std::map;
	using ::std::move;
	using ::std::pair;
	using ::std::remove_const_t;
	using ::std::remove_reference_t;
	using ::std::shared_ptr;
	using ::std::size;
	using ::std::size_t;
	using ::std::string;
	using ::std::tuple_element;
	using ::std::tuple_size_v;
	using namespace Store;

	/// ByteConverter 的工作是将一个类型和 Byte 之间相互转换，这里代码都很适合静态反射类所含成员去做
	/// TypeConverter 的工作是将一个转换为适合硬盘读取的类型（可以有类似 DiskPtr 这样的读取隔离，而不是一下子读取完）

	template <typename T, bool>
	struct ByteConverter;

	template <typename T>
	struct GetSizeStable
	{
		static constexpr bool Result = ByteConverter<T>::SizeStable;
	};

	template <typename T>
	struct GetSize
	{
		static constexpr size_t Result = ByteConverter<T>::Size;
	};

	template <typename T>
	struct ByteConverter<T, true>
	{
		static constexpr bool SizeStable = true;
		static constexpr size_t Size = sizeof(T);

		static void WriteDown(T const& t, IWriter auto* writer) 
		{
			char const* start = reinterpret_cast<char const*>(&t);
			writer->Add(start, sizeof(T));
		}

		static T ReadOut(IReader auto* reader)
		{
			auto bytes = reader->template Read<sizeof(T)>();
			T* p = reinterpret_cast<T*>(&bytes[0]);
			return *p;
		}
	};


	template <typename T>
	struct RemoveRefConstInTuple;

	using ::std::tuple;
	template <typename... Ts>
	struct RemoveRefConstInTuple<tuple<Ts...>>
	{
		// 下面这个 ... 居然可以，我还以为是返回最后一个类型的结果呢
		// 那 Btree 的获得 NodeBase 参数可以这么搞吗？
		using Result = tuple<remove_const_t<remove_reference_t<Ts>>...>;// 这里对应着我们常用的 Ts...，试一下在函数参数里面展开 TODO
	};

	template <typename T>
	struct ByteConverter<T, false>
	{
		// RemoveRefConstInTuple 呼应 converter 里面的两个 remove...
		using Tuple = typename RemoveRefConstInTuple<typename ReturnType<decltype(ToTuple<T>)>::Type>::Result;
		static constexpr bool SizeStable = All<GetSizeStable, Tuple>::Result;
		static constexpr size_t Size = SizeStable ? Sum<GetSize, Tuple>::Result : SIZE_MAX;

		template <typename Ty, auto... Is>
		static void WriteEach(IWriter auto* writer, Ty const& tup, index_sequence<Is...>)
		{
			auto converter = [&]<auto Index>()
			{
				auto& i = get<Index>(tup);
				ByteConverter<remove_const_t<remove_reference_t<decltype(i)>>>::WriteDown(i, writer);
			};

			(converter.template operator()<Is>(), ...);
		}

		static void WriteDown(T const& t, IWriter auto* writer)
		{
			auto tup = ToTuple(forward<decltype(t)>(t));
			WriteEach(writer, tup, make_index_sequence<tuple_size_v<decltype(tup)>>());
		}

		template <typename Ty, typename UnitConverter, auto... Is>
		static Ty ConsT(UnitConverter converter, index_sequence<Is...>)
		{
			return { converter.template operator ()<Is>()... };
		}

		static T ReadOut(IReader auto* reader)
		{
			auto converter = [&]<auto Index>()
			{
				using SubType = typename tuple_element<Index, Tuple>::type;
				return ByteConverter<remove_reference_t<SubType>>::ReadOut(reader);
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
		using ThisType = T;

		static void WriteDown(T const& t, IWriter auto* writer)
		{
			size_t n = t.size();
			ByteConverter<decltype(n)>::WriteDown(n, writer);
			writer->Add(t.c_str(), n);
		}

		static T ReadOut(IReader auto* reader)
		{
			size_t charCount = ByteConverter<size_t>::ReadOut(reader);
			auto bytes = reader->Read(charCount);
			char* str = reinterpret_cast<char*>(bytes.data());
			return { str, str + charCount };
		}
	};

	template <typename T, typename size_int, size_int Capacity>
	struct ByteConverter<LiteVector<T, size_int, Capacity>, false>
	{
		static constexpr bool SizeStable = GetSizeStable<T>::Result;
		static constexpr size_t Size = SizeStable ? Sum<GetSize, T>::Result * Capacity : SIZE_MAX;
		using ThisType = LiteVector<T, size_int, Capacity>;

		static void WriteDown(ThisType const& vec, IWriter auto* writer)
		{
			// Count
			auto n = vec.Count();
			ByteConverter<size_int>::WriteDown(n, writer);
			// Items
			for (auto& t : vec)
			{
				ByteConverter<T>::WriteDown(t, writer);
			}

			if constexpr (SizeStable)
			{
				auto unitSize = Size / Capacity;
				writer->AddBlank(unitSize * (Capacity - n));
			}
		}

		static ThisType ReadOut(IReader auto* reader)
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
		static constexpr size_t Size = SizeStable ? Sum<GetSize, Key, Value>::Result : SIZE_MAX;

		static void WriteDown(ThisType const& t, IWriter auto* writer)
		{
			ByteConverter<Key>::WriteDown(t.first, writer);
			ByteConverter<Value>::WriteDown(t.second, writer);
		}

		static ThisType ReadOut(IReader auto* reader)
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
		static constexpr size_t Size = Sum<GetSize, BaseType>::Result;

		static void WriteDown(ThisType const& t, IWriter auto* writer)
		{
			ByteConverter<BaseType>::WriteDown(t, writer);
		}

		static ThisType ReadOut(IReader auto* reader)
		{
			// Each type should have a constructor of all data member to easy set
			// Like Elements have a constructor which has LiteVector as arg
			return ByteConverter<BaseType>::ReadOut(reader);
		}
	};

	template <typename Key, typename Value>
	struct ByteConverter<map<Key, Value>, false>
	{
		using ThisType = map<Key, Value>;
		static constexpr bool SizeStable = false;

		static void WriteDown(ThisType const& t, IWriter auto* writer)
		{
			size_t size = t.size();
			ByteConverter<size_t>::WriteDown(size, writer);

			for (auto& item : t)
			{
				ByteConverter<pair<Key, Value>>::WriteDown(item, writer);
			}
		}

		static ThisType ReadOut(IReader auto* reader)
		{
			size_t size = ByteConverter<size_t>::ReadOut(reader);
			ThisType t;
			for (auto i = 0; i < size; ++i)
			{
				auto item = ByteConverter<pair<Key, Value>>::ReadOut(reader);
				t.insert(move(item));
			}
			
			return t;
		}
	};

	template <typename T>
	struct ByteConverter<vector<T>, false>
	{
		using ThisType = vector<T>;
		static constexpr bool SizeStable = false;

		static void WriteDown(ThisType const& t, IWriter auto* writer)
		{
			size_t size = t.size();
			ByteConverter<size_t>::WriteDown(size, writer);

			for (auto& item : t)
			{
				ByteConverter<T>::WriteDown(item, writer);
			}
		}

		static ThisType ReadOut(FileReader *reader)
		{
			size_t size = ByteConverter<size_t>::ReadOut(reader);
			ThisType t;
			for (auto i = 0; i < size; ++i)
			{
				auto item = ByteConverter<T>::ReadOut(reader);
				t.push_back(move(item));
			}

			return t;
		}
	};
}