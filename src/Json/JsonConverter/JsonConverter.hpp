#pragma once
#include <string>
#include <type_traits>
#include <tuple>
#include <utility>
#include <memory>
#include <vector>
#include <array>
#include <map>
#include "../Json.hpp"

namespace Json::JsonConverter
{
	using ::std::array;
	using ::std::decay_t;
	using ::std::declval;
	using ::std::false_type;
	using ::std::forward;
	using ::std::make_shared;
	using ::std::make_tuple;
	using ::std::map;
	using ::std::shared_ptr;
	using ::std::string;
	using ::std::true_type;
	using ::std::vector;

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
	auto ToTuple(T&& object) noexcept
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

	template <typename T>
	JsonObject Serialize(T const& t);
	// {
		// static_assert(false, "not support type");
	// }
	// why can not write like this?

	template <>
	JsonObject Serialize<string>(string const& t);

	template <>
	JsonObject Serialize(int const& t);

	template <>
	JsonObject Serialize(double const& t);

	template <>
	JsonObject Serialize(bool const& t);

	template <typename T>
	JsonObject Serialize(vector<T> const& t)
	{
		vector<shared_ptr<JsonObject>> vec;
		vec.reserve(t.size());

		for (auto& i : t)
		{
			vec.emplace_back(make_shared<JsonObject>(Serialize(i)));
		}

		return JsonObject(vec);
	}

	template <typename T, auto Count>
	JsonObject Serialize(array<T, Count> const& t)
	{
		vector<shared_ptr<JsonObject>> vec;
		vec.reserve(t.size());

		for (auto& i : t)
		{
			vec.emplace_back(make_shared<JsonObject>(Serialize(i)));
		}

		return JsonObject(vec);
	}

	template <typename Value>
	JsonObject Serialize(map<string, Value> const& t)
	{
		map<string, shared_ptr<JsonObject>> m;

		for (auto& p : t)
		{
			m.insert({ p.first, make_shared<JsonObject>(Serialize(p.second)) });
		}

		return JsonObject(m);
	}

	template <typename T>
	T Deserialize(JsonObject const&);

	template <>
	string Deserialize(JsonObject const& json);

	template <>
	int Deserialize(JsonObject const& json);

	template <>
	double Deserialize(JsonObject const& json);

	template <>
	bool Deserialize(JsonObject const& json);

	template <typename T>
	vector<T> DeserializeImp(JsonObject const& json, vector<T>*)
	{
		vector<T> des;
		for (auto& objPtr : json.GetArray())
		{
			des.push_back(Deserialize<T>(*objPtr));
		}

		return des;
	}

	template <typename Value>
	map<string, Value> DeserializeImp(JsonObject const& json, map<string, Value>*)
	{
		map<string, Value> des;

		for (auto& p : json.GetObject())
		{
			des.insert({ p.first, Deserialize<Value>(*p.second) });
		}

		return des;
	}

	template <typename T>
	T Deserialize(JsonObject const& json)
	{
		using type = decay_t<T>;
		return DeserializeImp(json, static_cast<type*>(nullptr));
	}
}
