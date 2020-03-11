#pragma once
#include <string>
#include <type_traits>
#include <tuple>
#include "Json.hpp"

namespace Json
{
	using ::std::string;
	using ::std::false_type;
	using ::std::true_type;
	using ::std::declval;
	using ::std::make_tuple;
	
	template <typename T, typename... Args>
	decltype(void(T{declval<Args>()...}), true_type())
	test(int);

	template <typename T, typename... Args>
	false_type
	test(...); //... mean ?

	template <typename T, typename... Args>
	struct IsBracesConstructible : decltype(test<T, Args...>(0))
	{ };

	struct AnyType
	{
		template <typename T>
		operator T();		
	};
	
	template <typename T>
	auto ToTuple(T&& objct)
	{
		if constexpr (IsBracesConstructible<T, AnyType, AnyType>())
		{
			auto&& [p1, p2] = objct;
			return make_tuple(p1, p2);
		}
		else if constexpr 
	}

	class JsonConverter
	{
	public:
		template <typename T>
		static string Serialize(T const& t)
		{
			
		}

		template <typename T>
		static T Deserialize(string const& jsonStr)
		{
			// 在 T 中实现 deserialize 的方法，或者一些相关信息的代码
			// make T class data member as tuple and iterate them
			// each item corresponding a Deserialize specialization function
		}

		template <typename T>
		static T Deserialize(JsonObject const& json)
		{

		}
	};
}
