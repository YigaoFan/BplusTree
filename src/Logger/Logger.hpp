#pragma once
#include <string>
#include <string_view>
#include "FormatMap.hpp"
#include "FormatParser.hpp"

namespace Log
{
	using ::std::string;

	// below code from: https://ctrpeach.io/posts/cpp20-string-literal-template-parameters/

	template <typename... InfoTypes>
	class Logger
	{
	private:
		// compile time format
		// 每天存档前一日的 log 文件，触发 log 操作的时候检查下，或者能设置定时回调吗？设个定时任务
		// "%h %l %u %t \"%r\" %>s %b \"%{Referer}i\" \"%{User-agent}i\""
	public:
		Logger(/* args */)
		{

		}

		void Info(string);
		void Info(InfoTypes...);
		void Warn(InfoTypes...);
		void Error(InfoTypes...);
	};

	template <typename TypeList, size_t... Idxs>
	auto DoMakeLogger(TypeList, index_sequence<Idxs...>)
	{
		return Logger<typename Get<TypeList, Idxs>::Result...>();
	}

	template <char... FormatChars>
	auto MakeLoggerWith(integer_sequence<char, FormatChars...> format)
	{
		auto typeList = ParseFormat(format);
		using TypeList = decltype(typeList);
		constexpr int size = Length<TypeList>::Result;
		auto idxs = make_index_sequence<size>();
		return DoMakeLogger(typeList, idxs);
	}
}
