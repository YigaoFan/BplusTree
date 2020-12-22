#pragma once
#include <chrono>
#include <string>

namespace Log
{
	using ::std::string;

	template <char Char>
	struct FormatMap;

	// 那些要加双引号或者其他东西的规定应该放在这里的类里面

	/// Hostname or ip address
	template <>
	struct FormatMap<'h'>
	{
		using To = string;
	};

	/// Username
	template <>
	struct FormatMap<'u'>
	{
		using To = string;
	};

	/// Request id
	template <>
	struct FormatMap<'i'>
	{
		using To = string;
	};

	/// Timestamp
	template <>
	struct FormatMap<'t'>
	{
		using To = ::std::chrono::time_point<::std::chrono::system_clock>;
	};

	/// Request function 
	template <>
	struct FormatMap<'r'>
	{
		using To = string;
	};

	/// Request argument
	template <>
	struct FormatMap<'a'>
	{
		using To = string;
	};

	enum class ResultStatus
	{
		Undefine = -1,
		Complete,
		Failed,
	};

	/// Request status code after process
	template <>
	struct FormatMap<'s'>
	{
		using To = ResultStatus;
	};
}
