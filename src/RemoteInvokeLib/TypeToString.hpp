#pragma once
#include <map>
#include <array>
#include <string>
#include <vector>

namespace RemoteInvokeLib
{
	using ::std::array;
	using ::std::map;
	using ::std::string;
	using ::std::vector;

#define nameof(NAME) #NAME

	template <typename T>
	void TypeToString()
	{
		static_assert(false, "Not support type");
	}

	template <>
	string TypeToString<string>()
	{
		return nameof(string);
	}

	template <>
	string TypeToString<int>()
	{
		return nameof(int);
	}

	template <>
	string TypeToString<bool>()
	{
		return nameof(bool);
	}

	template <>
	string TypeToString<double>()
	{
		return nameof(double);
	}

	template <typename T>
	string TypeToString<vector<T>>()
	{
		return string(nameof(vector)"<").append(TypeToString<T>()).append('>');
	}

	template <typename T, size_t N>
	string TypeToString<array<T, N>>()
	{
		return string(nameof(array) "<")
				.append(TypeToString<T>())
				.append(',')
				.append(std::to_string(N))
				.append('>');
	}

	template <typename Key, typename Value>
	string TypeToString<map<Key, Value>>()
	{
		return string(nameof(map) "<").append(TypeToString<T>()).append('>');
	}

#undef nameof
}
