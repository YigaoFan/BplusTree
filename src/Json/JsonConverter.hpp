#pragma once
#include <string>
#include <type_traits>
#include <tuple>
#include <utility>
#include "Json.hpp"

namespace Json
{
	using ::std::declval;
	using ::std::declval;
	using ::std::false_type;
	using ::std::forward;
	using ::std::make_tuple;
	using ::std::string;
	using ::std::true_type;

	// Below code to ToTuple inspire from Chris Ohk
	// https://gist.github.com/utilForever/1a058050b8af3ef46b58bcfa01d5375d
	template <typename T, typename... Args>
	decltype(void(T{declval<Args>()...}), true_type())
	TestBracesConstructible(int);

	template <typename, typename...>
	false_type
	TestBracesConstructible(...); //... mean ?

	template <typename T, typename... Args>
	using IsBracesConstructible = decltype(TestBracesConstructible<T, Args...>(0));

	struct AnyType
	{
		template <typename T>
		constexpr operator T();
	};

	template <typename T>
	auto ToTuple(T &&object) noexcept
	{
		using type = decay_t<T>;
		// Here can use Boost macro replaced
		if constexpr (IsBracesConstructible<type, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType>())
		{
			auto&& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30] = forward<T>(object);
			return make_tuple(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30);
		}
		else if constexpr (IsBracesConstructible<type, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType>())
		{
			auto&& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29] = forward<T>(object);
			return make_tuple(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29);
		}
		else if constexpr (IsBracesConstructible<type, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType>())
		{
			auto&& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28] = forward<T>(object);
			return make_tuple(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28);
		}
		else if constexpr (IsBracesConstructible<type, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType>())
		{
			auto&& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27] = forward<T>(object);
			return make_tuple(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27);
		}
		else if constexpr (IsBracesConstructible<type, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType>())
		{
			auto&& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26] = forward<T>(object);
			return make_tuple(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26);
		}
		else if constexpr (IsBracesConstructible<type, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType>())
		{
			auto&& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25] = forward<T>(object);
			return make_tuple(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25);
		}
		else if constexpr (IsBracesConstructible<type, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType>())
		{
			auto&& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24] = forward<T>(object);
			return make_tuple(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24);
		}
		else if constexpr (IsBracesConstructible<type, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType>())
		{
			auto&& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23] = forward<T>(object);
			return make_tuple(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23);
		}
		else if constexpr (IsBracesConstructible<type, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType>())
		{
			auto&& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22] = forward<T>(object);
			return make_tuple(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22);
		}
		else if constexpr (IsBracesConstructible<type, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType>())
		{
			auto&& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21] = forward<T>(object);
			return make_tuple(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21);
		}
		else if constexpr (IsBracesConstructible<type, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType>())
		{
			auto&& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20] = forward<T>(object);
			return make_tuple(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20);
		}
		else if constexpr (IsBracesConstructible<type, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType>())
		{
			auto&& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19] = forward<T>(object);
			return make_tuple(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19);
		}
		else if constexpr (IsBracesConstructible<type, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType>())
		{
			auto&& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18] = forward<T>(object);
			return make_tuple(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18);
		}
		else if constexpr (IsBracesConstructible<type, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType>())
		{
			auto&& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17] = forward<T>(object);
			return make_tuple(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17);
		}
		else if constexpr (IsBracesConstructible<type, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType>())
		{
			auto&& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16] = forward<T>(object);
			return make_tuple(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16);
		}
		else if constexpr (IsBracesConstructible<type, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType>())
		{
			auto&& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15] = forward<T>(object);
			return make_tuple(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15);
		}
		else if constexpr (IsBracesConstructible<type, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType>())
		{
			auto&& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14] = forward<T>(object);
			return make_tuple(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14);
		}
		else if constexpr (IsBracesConstructible<type, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType>())
		{
			auto&& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13] = forward<T>(object);
			return make_tuple(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13);
		}
		else if constexpr (IsBracesConstructible<type, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType>())
		{
			auto&& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12] = forward<T>(object);
			return make_tuple(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12);
		}
		else if constexpr (IsBracesConstructible<type, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType>())
		{
			auto&& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11] = forward<T>(object);
			return make_tuple(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11);
		}
		else if constexpr (IsBracesConstructible<type, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType>())
		{
			auto&& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10] = forward<T>(object);
			return make_tuple(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10);
		}
		else if constexpr (IsBracesConstructible<type, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType>())
		{
			auto&& [p1, p2, p3, p4, p5, p6, p7, p8, p9] = forward<T>(object);
			return make_tuple(p1, p2, p3, p4, p5, p6, p7, p8, p9);
		}
		else if constexpr (IsBracesConstructible<type, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType>())
		{
			auto&& [p1, p2, p3, p4, p5, p6, p7, p8] = forward<T>(object);
			return make_tuple(p1, p2, p3, p4, p5, p6, p7, p8);
		}
		else if constexpr (IsBracesConstructible<type, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType>())
		{
			auto&& [p1, p2, p3, p4, p5, p6, p7] = forward<T>(object);
			return make_tuple(p1, p2, p3, p4, p5, p6, p7);
		}
		else if constexpr (IsBracesConstructible<type, AnyType, AnyType, AnyType, AnyType, AnyType, AnyType>())
		{
			auto&& [p1, p2, p3, p4, p5, p6] = forward<T>(object);
			return make_tuple(p1, p2, p3, p4, p5, p6);
		}
		else if constexpr (IsBracesConstructible<type, AnyType, AnyType, AnyType, AnyType, AnyType>())
		{
			auto&& [p1, p2, p3, p4, p5] = forward<T>(object);
			return make_tuple(p1, p2, p3, p4, p5);
		}
		else if constexpr (IsBracesConstructible<type, AnyType, AnyType, AnyType, AnyType>())
		{
			auto&& [p1, p2, p3, p4] = forward<T>(object);
			return make_tuple(p1, p2, p3, p4);
		}
		else if constexpr (IsBracesConstructible<type, AnyType, AnyType, AnyType>())
		{
			auto&& [p1, p2, p3] = forward<T>(object);
			return make_tuple(p1, p2, p3);
		}
		else if constexpr (IsBracesConstructible<type, AnyType, AnyType>())
		{
			auto&& [p1, p2] = forward<T>(object);
			return make_tuple(p1, p2);
		}
		else if constexpr (IsBracesConstructible<type, AnyType>())
		{
			auto&& [p1] = forward<T>(object);
			return make_tuple(p1);
		}
		else
		{
			return make_tuple();
		}
	}

	class JsonConverter
	{
		// using ::std::index_sequence;

	public:
		template <typename Tuple, size_t... Indexs>
		string SerializeEach(Tuple&& tuple, ::std::index_sequence<Indexs...>)
		{
			using ::std::get;
			return (Serialize(get<Indexs>(forward<Tuple>(tuple)))...);
		}

		template <typename T>
		static string Serialize(T const &t)
		{
			using ::std::tuple_size;
			using ::std::make_index_sequence;
			auto t = ToTuple(forward<T>(t)); // forward maybe use wrong
			auto is = make_index_sequence<tuple_size<remove_reference_t<decltype(t)>>()>{};
			return SerializeEach(move(t), is);
		}

		template <>
		static string Serialize<string>(string const &t)
		{
		}

		template <>
		static string Serialize<int>(int const &t)
		{
		}

		// https://stackoverflow.com/questions/2183087/why-cant-i-use-float-value-as-a-template-parameter
		//template <>
		//static string Serialize<float>(int const& t)
		//{
		//    // Convert struct to tuple
		//    // Iterate item ToString
		//}

		template <>
		static string Serialize<bool>(bool const &t)
		{
		}

		template <typename T>
		static T Deserialize(string const &jsonStr)
		{
			// 在 T 中实现 deserialize 的方法，或者一些相关信息的代码
			// make T class data member as tuple and iterate them
			// each item corresponding a Deserialize specialization function
		}

		template <typename T>
		static T Deserialize(JsonObject const &json)
		{
		}

		template <>
		static string Deserialize<string>(string const &jsonStr)
		{
		}

		template <>
		static int Deserialize(string const &jsonStr)
		{
		}

		template <>
		static bool Deserialize(string const &jsonStr)
		{
		}
	};
}
