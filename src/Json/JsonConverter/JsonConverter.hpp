#pragma once
#include <string>
#include <type_traits>
#include <tuple>
#include <utility>
#include <vector>
#include <array>
#include <map>
#include "../Json.hpp"

namespace Json::JsonConverter
{
	using ::std::array;
	using ::std::decay_t;
	using ::std::make_shared;
	using ::std::map;
	using ::std::pair;
	using ::std::string;
	using ::std::vector;

	template <typename T>
	JsonObject Serialize(T const& t);
	// {
		// static_assert(false, "not support type");
	// }
	// why can not write like this?
	// 看下模板函数偏特化的内容，然后把这里改一下 去掉 template<>
	template <>
	JsonObject Serialize(string const& t);

	template <>
	JsonObject Serialize(int const& t);

	template <>
	JsonObject Serialize(double const& t);

	template <>
	JsonObject Serialize(bool const& t);

	template <>
	JsonObject Serialize(JsonObject const& t);

#define nameof(VAR) #VAR
	template <typename T1, typename T2>
	JsonObject Serialize(pair<T1, T2> const& p)
	{
		auto [first, second] = p;
		JsonObject::_Object obj;
		obj.insert({nameof(first), Serialize(first)});
		obj.insert({nameof(second), Serialize(second)});

		return JsonObject(move(obj));
	}

	// 莫非下面函数里面的 Serialize(i) 会挑选这里之前的符合类型的函数？
	template <typename T>
	JsonObject Serialize(vector<T> const& t)
	{
		JsonObject::_Array vec;
		vec.reserve(t.size());

		for (auto& i : t)
		{
			vec.emplace_back(Serialize(i));
		}

		return JsonObject(move(vec));
	}

	template <typename T, auto Count>
	JsonObject Serialize(array<T, Count> const& t)
	{
		JsonObject::_Array vec;
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
		JsonObject::_Object m;

		for (auto& p : t)
		{
			m.insert({ p.first, Serialize(p.second) });
		}

		return JsonObject(move(m));
	}

	template <typename T>
	T Deserialize(JsonObject const&);

	template <>
	string Deserialize(JsonObject const& json);

	template <typename T1, typename T2>
	pair<T1, T2> Deserialize(JsonObject const &jsonObj)
	{
		auto first = Deserialize<T1>(jsonObj[nameof(first)]);
		auto second = Deserialize<T2>(jsonObj[nameof(second)]);

		return {move(first), move(second)};
	}
	
	template <>
	int Deserialize(JsonObject const& json);

	template <>
	double Deserialize(JsonObject const& json);

	template <>
	bool Deserialize(JsonObject const& json);

	template <typename Key, typename Value>
	pair<Key, Value> DeserializeImp(JsonObject const& json, pair<Key, Value>*)
	{
		return
		{
			Deserialize<Key>(json[nameof(first)]),
			Deserialize<Value>(json[nameof(second)]),
		};
	}

	template <typename T>
	vector<T> DeserializeImp(JsonObject const& json, vector<T>*)
	{
		vector<T> des;
		for (auto& obj : json.GetArray())
		{
			des.push_back(Deserialize<T>(obj));
		}

		return des;
	}

	template <typename T, size_t N>
	array<T, N> DeserializeImp(JsonObject const &json, array<T, N> *)
	{
		using ::std::index_sequence;
		using ::std::make_index_sequence;

		auto a = json.GetArray();
		auto consArray = [&a]<auto... Idxs>(index_sequence<Idxs...>)
		{
			return array<T, N>
			{
				Deserialize<T>(a[Idxs])...,
			};
		};

		return consArray(make_index_sequence<N>());
	}

	template <typename Value>
	map<string, Value> DeserializeImp(JsonObject const& json, map<string, Value>*)
	{
		map<string, Value> des;

		for (auto& p : json.GetObject())
		{
			des.insert({ p.first, Deserialize<Value>(p.second) });
		}

		return des;
	}

	template <typename T>
	T Deserialize(JsonObject const& json)
	{
		using type = decay_t<T>;
		return DeserializeImp(json, static_cast<type*>(nullptr));
	}
#undef nameof
}
