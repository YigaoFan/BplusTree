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
	struct TypeToString
	{
		static void Result()
		{
		}
	};

	template <>
	struct TypeToString<string>
	{
		static string Result()
		{
			return nameof(string);
		}
	};

	template <>
	struct TypeToString<int>
	{
		static string Result()
		{
			return nameof(int);
		}
	};

	template <>
	struct TypeToString<bool>
	{
		static string Result()
		{
			return nameof(bool);

		}
	};

	template <>
	struct TypeToString<double>
	{
		static string Result()
		{
			return nameof(double);
		}
	};

	template <typename T>
	struct TypeToString<vector<T>>
	{
		static string Result()
		{
			return string(nameof(vector)"<").append(TypeToString<T>()).append('>');
		}
	};

	template <typename T, size_t N>
	struct TypeToString<array<T, N>>
	{
		static string Result()
		{
			return string(nameof(array) "<")
				.append(TypeToString<T>())
				.append(',')
				.append(std::to_string(N))
				.append('>');
		}
	};

	template <typename Key, typename Value>
	struct TypeToString<map<Key, Value>>
	{
		static string Result()
		{
			return string(nameof(map) "<")
				.append(TypeToString<Key>())
				.append(',')
				.append(TypeToString<Value>())
				.append('>');
		}
	};

#undef nameof
}
