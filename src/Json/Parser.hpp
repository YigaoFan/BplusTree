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
		string _str;

		Parser(string str)
			: _str(str)
		{ }

		Json
		doParse(size_t start, size_t end)
		{
			while (true) {
				auto currentExpect; // 是一个类型，然后有一系列函数可以将一个 char 或者某个输入归为某个类型
				auto currentSpaceHandle; // Could capture current environment
				auto c = _str[i++];
				// 有语法错误直接抛异常出来，应该没有 try-catch 捕获异常
				switch (c) {
					case '{':
						/* object，这里是否会涉及到 null？*/
						auto subEnd = verifyRightBound('}', end);
						return parseObject(i, end);

					case '[':
						/* array */
						return parseArray(i);

					case '"':
						/* string（暂时只支持双引号字符串） */
						return parseString(i);

					case 't':
						/* true */
						return parseTrue(i);

					case 'f':
						/* false */
						return parseFalse(i);

					case 'n':
						/* null */
						return parseNull(i);

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

		// 前提是这个是最上层的 JSON 对象
		Json
		parseObject(size_t& i)
		{
			// search pair and judge
			auto pairBracket = verifyCurlyBracket(i);

			auto key = parseString(i);
			// how to get value end point
			auto value = doParse();
			// handle ,
			// continue parse key-value
		}

		Json
		parseArray(size_t& i)
		{
			// search pair and judge			
		}

		Json
		parseString(size_t& start, size_t rightBound)
		{
			auto str = "";

			// 一个约定是每一个解析函数处理完后，需要将 i 置到下一个不合法的位置上
			while (i < _str.length() && (auto c = _str[i++]) !=  '"') {
				str.append(c);
				// 暂时不处理转义的事
			}

			return Json<String>(std::move(str));
		}

		inline
		Json
		parseTrue(size_t& i)
		{
			return parseSimpleType<True>(TrueStr, i);
		}

		inline
		Json
		parseFalse(size_t& i)
		{
			return parseSimpleType<False>(FalseStr, i);
		}

		inline
		Json
		parseNull(size_t& i)
		{
			return parseSimpleType<Null>(NullStr, i);
		}

		Json
		parseNum(size_t& i)
		{
			
		}

		template <typename T>
		Json
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

			return Json<T>();
		}

		// 假设是在 JSON 解析的最高层，所以直接从最后开始
		size_t
		verifyCurlyBracket(size_t i)
		{
			auto end = _str.rend();
			for (auto i = _str.length()-1; i >= 0; --i) {
				auto c = _str[i];
				if (!isSpace(c)) {
					if (c != '}') {
						throw WrongCharException(c);
					} else {
						return i;
					}
				}
			}
		}

		size_t
		verifyRightBound(size_t start, size_t end, char expectChar)
		{
			for (auto i = start; i <= end; ++i) {
				if ((auto c = _str[i]) == expectChar) {
					return i;
				}
			}
			// throw WrongException
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