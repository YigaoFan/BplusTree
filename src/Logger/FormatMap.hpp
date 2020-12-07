#pragma once
#include <chrono>
#include <string>

namespace Log
{
	using ::std::string;

	template <char Char>
	struct FormatMap;

	// 那些要加双引号或者其他东西的规定应该放在这里的类里面
	template <>
	struct FormatMap<'h'>
	{
		using To = string;
	};

	template <>
	struct FormatMap<'l'>
	{
		using To = string;
	};

	template <>
	struct FormatMap<'u'>
	{
		using To = string;
	};

	template <>
	struct FormatMap<'t'>
	{
		using To = ::std::chrono::time_point<::std::chrono::system_clock>;
	};

	template <>
	struct FormatMap<'r'>
	{
		using To = string;
	};

	template <>
	struct FormatMap<'s'> // >s
	{
		using To = int;
	};

	template <>
	struct FormatMap<'b'>
	{
		using To = size_t;
	};

	template <>
	struct FormatMap<'i'>
	{
		using To = string;
	};
}
