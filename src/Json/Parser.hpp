#pragma once
#include <string>
#include <vector>
#include <cctype>
#include "ParseException.hpp"
#include "Json.hpp"

namespace Json {
	using std::string;

	// 我想的是有针对当前正在处理的对象的情境这个概念，
	// 比如当前正在处理一个空格（也就是说现在不进行去除空格的处理了），如果是在字符串里的空格，那就加入进去
	// 如果是项与项之间的，就忽略，如果是其他项内的空格，则报错
	// 还有一个当前期望的东西存在，如果是数字则期望数字，这样不是数字则能比较容易和清楚的报出语法错误
	class Parser {
	private:
		const string TrueStr = "rue";
		const string FalseStr = "alse";
		const string NullStr = "ull";
		const string& _str;

		Parser(const string& str)
			: _str(str)
		{ }

		Json
		doParse(size_t& start, size_t end)
		{
			while (true) {
				auto currentExpect; // 是一个类型，然后有一系列函数可以将一个 char 或者某个输入归为某个类型
				auto currentSpaceHandle; // Could capture current environment
				auto i = start;
				auto c = _str[i++];
				// 有语法错误直接抛异常出来，应该没有 try-catch 捕获异常
				switch (c) {
					case '{':
						auto subEnd = verifyRightBound(i, end, '}');
						return Json(Object(), parseObject(i, subEnd));

					case '[':
						auto subEnd = verifyRightBound(i, end, ']');
						return Json(Array(), parseArray(i));

					case '"':
						/* string（暂时只支持双引号字符串） */
						return Json(String(), parseString(i));

					case 't':
						parseTrue(i);
						return Json(True());

					case 'f':
						parseFalse(i);
						return Json(False());

					case 'n':
						parseNull(i);
						return Json(Null());

					default:
					CheckSpace:
						if (isSpace(c)) {
							++i;
							goto CheckSpace;
						} else if (isNum(c)) {
							return parseNum(i);
						} else {
							// error, current expect is ...
							throw InvalidStringException();
						}
				}
			}
		}

		inline
		static
		bool
		isSpace(char c)
		{
			 // check if work on which char
			return std::isblank(static_cast<unsigned char>(c));
		}

		inline
		static
		bool
		isNum(char c)
		{
			return std::isdigit(static_cast<unsigned char>(c));
		}

		bool
		inJsonString(size_t i)
		{
			// search around
		}

		map<string, Json*>
		parseObject(size_t& start, size_t end)
		{
			auto& i = start;
			while (isSpace(_str[i])) { ++i; };
			// 是直接在 Object 里存储 string 还是存储 Json string
			// 有嵌套、扩展能力的地方都用 Json，嵌套部分中不可扩展的部分
			// 使用原来的类型
			map<string, Json> objectMap;
			auto key = parseString(i, end);
			auto value = doParse(i, end);
			objectMap.emplace(key, &value); // use smart ptr
		}

		vector<Json*>
		parseArray(size_t& i)
		{
			// search pair and judge	
		}

		string
		parseString(size_t& start, size_t end)
		{
			string str { };
			for (auto& i = start; i < end && _str[i] != '"'; ++i) {
				auto c = _str[i];
				str.append(1, c);
			}

			auto i = start;
			if (_str[i] == '"') {
				return str;
			} else {
				// throw Wrong...Exception
			}
		}

		inline
		void
		parseTrue(size_t& i)
		{
			parseSimpleType<True>(TrueStr, i);
		}

		inline
		void
		parseFalse(size_t& i)
		{
			parseSimpleType<False>(FalseStr, i);
		}

		inline
		void
		parseNull(size_t& i)
		{
			parseSimpleType<Null>(NullStr, i);
		}

		Json
		parseNum(size_t& i)
		{
			
		}

		template <typename T>
		void
		parseSimpleType(const string& target, size_t& i)
		{
			auto len = target.length();
			for (int j = 0; j < len; ++j, ++i) {
				auto t = target[j];
				auto c = _str[i];
				if (t != s) {
					throw WrongCharException(c);
				}
			}
		}

		// 这里的 bound 都是不包含 expectChar 的
		size_t
		verifyRightBound(size_t start, size_t end, char expectChar)
		{
			for (auto i = end; i >= start; --i) {
				if ((auto c = _str[i]) == expectChar) {
					return --i;
				}
			}

			throw WrongPairException();
		}

	public:
		Json
		parse(string jsonStr)
		{
			auto e = jsonStr.length() - 1;
			auto p = Parser(std::move(jsonStr));
			p.doParse(0, e);
		}
	};

	// may use namespace to hide some functions
	namespace {

	}
}