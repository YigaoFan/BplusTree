#pragma once
#include <cstddef>
#include <type_traits>
#include <utility>
#include <tuple>
#include <array>
#include "StructToTuple.hpp"

namespace FuncLib
{
	using ::std::is_trivial_v;
	using ::std::is_standard_layout_v;
	using ::std::is_class_v;
	using ::std::forward;
	using ::std::size_t;
	using ::std::index_sequence;
	using ::std::make_index_sequence;
	using ::std::tuple_size_v;
	using ::std::byte;
	using ::std::size;
	
	template <typename T, bool = is_standard_layout_v<T> && is_trivial_v<T>>
	struct ConvertToDiskType;

	// string 的特化里提供定长接口，大部分的类型应该都要提供这个接口
	template <typename T>
	struct ConvertToDiskType<T, false>
	{
		static_assert(is_class_v<T>, "Only support class type when not specialize");

		template <size_t... Is>
		auto ForEachConvert(index_sequence<Is...>)
		{
			array<byte, 1> a;
			array<byte, 1> b;
			array<byte, 1> c;

			auto Convert = template <auto Index>[/*TODO*/]()
			{

			};

			Convert.operator() <10>();
			constexpr size_t count = size(a) + size(b) + size(c);
			array<byte, count> d;
			// Copy each item
			return d;
		}

		auto Temp(T&& t)
		{
			auto tup = ToTuple(forward<T>(t));// TODO check forward use right?
			ForEachConvert(make_index_sequence<tuple_size_v<decltype(tup)>>());
		}
	};

	template <typename T>
	struct ConvertToDiskType<T, true>
	{
		using Type = T;


	};
}