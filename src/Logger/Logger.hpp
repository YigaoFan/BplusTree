#include <string>
#include <string_view>
#include "FormatMap.hpp"
#include "FormatParser.hpp"

namespace Log
{
	using Basic::TypeList;
	using ::std::string;

	// below code from: https://ctrpeach.io/posts/cpp20-string-literal-template-parameters/

	template <typename... InfoTypes>
	class Logger
	{
	private:
		// compile time format
		// 每天存档前一日的 log 文件
		// "%h %l %u %t \"%r\" %>s %b \"%{Referer}i\" \"%{User-agent}i\""
	public:
		Logger(/* args */);
		// Where to set format
		void Info(InfoTypes...);
		void Warn(InfoTypes...);
		void Error(InfoTypes...);
	};

	template <char... FormatChars>
	auto MakeLoggerWith(integer_sequence<char, FormatChars...> format)
	{
		ParseFormat(format);
		return Logger<FormatMap<FormatChars>::To...>();
	}
}
