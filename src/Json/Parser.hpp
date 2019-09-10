#pragma once
#include <string>
#include <vector>
#include <cctype>
#include "Json.hpp"

namespace Json {
	using std::string;

	// 我想的是有针对当前正在处理的对象的情境这个概念，
	// 比如当前正在处理一个空格（也就是说现在不进行去除空格的处理了），如果是在字符串里的空格，那就加入进去
	// 如果是项与项之间的，就忽略，如果是其他项内的空格，则报错
	// 还有一个当前期望的东西存在，如果是数字则期望数字，这样不是数字则能比较容易和清楚的报出语法错误
	class Parser {
	private:
		string _str;

		Parser(string str)
			: _str(str)
		{ }

		Json
		doParse()
		{
			for (size_t i = 0; i < _str.length(); i++)
			{
				auto currentExpect; // 是一个类型，然后有一系列函数可以将一个 char 或者某个输入归为某个类型
				auto currentSpaceHandle; // Could capture current environment
				auto c = _str[i];
				switch (c)
				{
				case '{':
					/* object，这里是否会涉及到 null？*/
					return parseObject(i);
				
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
					if (isSpace(c)) {
						++i;
					} else if (isNum(c)) {
						return parseNum(i);
					} else {
						// error, current expect is ...
					}
				}
			}
		}
		
		static
		inline
		bool
		isSpace(char c)
		{
			 // check if work on which char
			return std::isblank(static_cast<unsigned char>(c));
		}

		static
		inline
		bool
		isNum(char c)
		{
			return std::isdigit(static_cast<unsigned char>(c));
		}

		static
		bool
		inJsonString(uint16_t i)
		{
			// search around
		}

		Json
		parseObject(size_t& i)
		{
			// search pair and judge
		}

		Json
		parseArray(size_t& i)
		{
			// search pair and judge			
		}

		Json
		parseString(size_t& i)
		{
			// search pair and judge			
		}

		Json
		parseTrue(size_t& i)
		{
			
		}

		Json
		parseFalse(size_t& i)
		{
			
		}

		Json
		parseNull(size_t& i)
		{
			
		}

		Json
		parseNum(size_t& i)
		{
			
		}
	};

	// may use namespace to hide some functions
	namespace {

	}
}