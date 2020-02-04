#pragma once
#include <string>
#include <vector>
#include <cctype>
#include <memory>
#include <utility>
#include <cstdlib>
#include <cerrno>
#include <cmath>
#include "ParseException.hpp"
#include "Json.hpp"
#include "LocationInfo.hpp"

namespace Json 
{
	using ::std::string;
	using ::std::make_shared;
	using ::std::pair;
	using ::std::make_pair;
	using ::std::size_t;
	using ::std::strtod;

	class Parser 
	{
	public:
		static JsonObject parse(string const& jsonStr)
		{
			return Parser(jsonStr).doParse();
		}

		/**
		 * should ensure the string to be parsed exists in current scope
		 */
		explicit Parser(const string& str) : Str(str)
		{ }

		JsonObject doParse()
		{
			_parsingLocation = 0;
			if (auto len = Str.length())
			{
				auto& i = _parsingLocation;
				auto t = detectForwardUnitType(i, len - 1);
				// "accurateEnd" means accurate bound
				auto accurateEnd = ensureSingleRoot(t, i, len - 1);
				return parseForwardUnit(t, i, accurateEnd);
			}

			throw ParseEmptyStringException();
		}

		// This method not suit here
		template <typename T>
		T deserialize(const JsonObject& json)
		{
			// 在 T 中实现 deserialize 的方法，或者一些相关信息的代码
		}

	private:
		const string TrueStr = "true";// TODO string_view?
		const string FalseStr = "false";
		const string NullStr = "null";
		const string& Str;
		size_t _parsingLocation = 0;

		JsonObject parseForwardUnit(JsonType parseType, size_t& after1stChar, size_t end)
		{
			// end here is just a bound, not represent one parse unit end
			// once a unit is parsed, methods below will return maybe not reaching the end
			switch (parseType) {
				case JsonType::Object:
					return JsonObject<JsonType::Object>(parseObject(after1stChar, end));

				case JsonType::Array:
					return JsonObject<JsonType::Array>(parseArray(after1stChar, end));

				case JsonType::Number:
					return JsonObject<JsonType::Number>(parseNum(after1stChar, end));

				case JsonType::String:
					return JsonObject<JsonType::String>(parseString(after1stChar, end));

				case JsonType::True:
					parseTrue(after1stChar);
					return JsonObject<JsonType::True>();

				case JsonType::False:
					parseFalse(after1stChar);
					return JsonObject<JsonType::False>();

				case JsonType::Null:
					parseNull(after1stChar);
					return JsonObject(Null());
			}
		}

		JsonObject forwardParseUnit(size_t& start, size_t bound)
		{
			auto type = detectForwardUnitType(start, bound);
			// 这里需要它顶层解析完就返回，当前的这种递归解析能保证吗
			return parseForwardUnit(type, start, bound);
		}

		// 所有的位置应该是偏向类型内的，比如 Object 的位置包含}，Number 都在 Number 内部上
		/**
		 * Ensure the string to be parsed is single root and return the accurate end of parsed one.
		 * @param unitType type of the root
		 * @param afterLeftPair the position after left paired position
		 * @param strEnd end of string to be parsed
		 * @return right paired position of the root
		 */
		size_t ensureSingleRoot(JsonType unitType, size_t afterLeftPair, size_t strEnd)
		{
			switch (unitType) {
				case JsonType::Object:
					return findRightPair(afterLeftPair, strEnd, '}');

				case JsonType::Array:
					return findRightPair(afterLeftPair, strEnd, ']');

				case JsonType::String:
					return findRightPair(afterLeftPair, strEnd, '"');

				case JsonType::Number:
					return findNumStrRightBound(afterLeftPair, strEnd);

#define REMAIN_LEN(stringName) (stringName.length() - 2)
				case JsonType::True:
					return ensureJustSimpleUnit(afterLeftPair + REMAIN_LEN(TrueStr), strEnd);

				case JsonType::False:
					return ensureJustSimpleUnit(afterLeftPair + REMAIN_LEN(FalseStr), strEnd);

				case JsonType::Null:
					return ensureJustSimpleUnit(afterLeftPair + REMAIN_LEN(NullStr), strEnd);
#undef REMAIN_LEN
			}
		}

		/**
		 * Detect forward unit parse type and move @param start to next position of the forward parse unit.
		 * @param start index to start detect
		 * @param strEnd index of detect range
		 * @return parsing type of unit
		 */
		JsonType detectForwardUnitType(size_t& start, size_t strEnd)
		{
			for (auto& i = start; i <= strEnd; ++i)
			{
				auto c = Str[i++];
				switch (c) 
				{
					case '{':
						return JsonType::Object;

					case '[':
						return JsonType::Array;

					case '"':
						return JsonType::String;

					case 't':
						return JsonType::True;

					case 'f':
						return JsonType::False;

					case 'n':
						return JsonType::Null;

					default:
						if (isSpace(c)) {
							continue;
						} else if (isNumStart(c)) {
							return JsonType::Number;
						} else {
							throw InvalidStringException(parsingLocationInfo());
						}
				}
			}

			throw ParseExpectValueException();
		}

		/**
		 * will check single root inner
		 * @return Index of expected char
		 */
		size_t findRightPair(size_t start, size_t end, char expected) const
		{
			for (auto i = end; i >= start; --i) {
				auto c = Str[i];
				if (c == expected) {
					return i;
				} else if (!isSpace(c)) {
					throw ParseNotSingleRootException(parsingLocationInfo());
				}
			}

			throw PairNotFoundException(start, end, expected);
		}

		/**
		 * Ensure just simple parse unit(include True, False, Null) from @param unitEnd to @param checkEnd
		 * @param unitEnd
		 * @param checkEnd
		 * @return @param unitEnd
		 */
		size_t ensureJustSimpleUnit(size_t unitEnd, size_t checkEnd) const
		{
			for (auto i = unitEnd + 1; i < checkEnd; ++i) {
				if (!isSpace(Str[i])) {
					throw ParseNotSingleRootException(locationInfoAt(i));
				}
			}

			return unitEnd;
		}

		size_t findNumStrRightBound(size_t start, size_t end) const
		{
			for (auto i = end; i >=  start; ++i) {
				auto c = Str[i];
				if (isNumTail(c)) {
					return i;
				} else if (!isSpace(c)) {
					throw ParseNotSingleRootException(parsingLocationInfo());
				}
			}
		}

		// 是否有个变量标识当前解析的类型，我想的是解析字符串的时候有些东西比较特殊，比如"
		/**
		 * Parse the object string without {} on both sides
		 */
		JsonObject::_Object parseObject(size_t& after1stChar, size_t end)
		{
			// 写了 JS 解析时间的代码，感觉这里状态的管理极可能是有问题的
			JsonObject::_Object objectMap;
			auto expectString = true, expectBracket = true, expectColon = false,
				expectJson = false, expectComma = false;
			string key;
			shared_ptr<JsonObject> value;

			for (auto& i = after1stChar; i <= end; ++i) {
				auto c = Str[i];
				if (isSpace(c)) {
					continue;
				} else if (expectString && c == '"') {
					key = parseString(++i, end);
					expectString = expectBracket = false;
					expectColon = true;
				} else if (expectColon && c == ':') {
					expectColon = false;
					expectJson = true;
				} else if (expectJson) {
					value = make_shared<JsonObject>(forwardParseUnit(i, end - 1));
					objectMap.emplace(std::move(key), std::move(value));
					key.clear(); value.reset();
					expectJson = false;
					expectComma = expectBracket = true;
				} else if (expectComma && c == ',') {
					// support ',' as end of object
					expectComma = false;
					expectString = expectBracket = true;
				} else if (expectBracket && c == '}') {
					++i;
					return objectMap;
				} else {
					throw InvalidStringException(parsingLocationInfo());
				}
			}

			throw ProgramError("now location is " + to_string(after1stChar) + " of ..." + parsingLocationInfo().charsAround());
		}

		JsonObject::_Array parseArray(size_t& after1stChar, size_t end)
		{
			JsonObject::_Array array;
			auto expectJson = true, expectSqrBracket = true, expectComma = false;

			for (auto& i = after1stChar; i <= end; ++i) {
				auto c = Str[i];
				if (isSpace(c)) {
					continue;
				} else if (expectSqrBracket && c == ']') {
					++i;
					return array;
				} else if (expectComma && c == ',') {
					// support ',' as end of array
					expectComma = false;
					expectJson = expectSqrBracket = true;
				} else if (expectJson) {
					// this and up object end is a too big range
					array.emplace_back(make_shared<JsonObject>(forwardParseUnit(i, end - 1)));
					expectJson = false;
					expectComma = expectSqrBracket = true;
				} else {
					throw InvalidStringException(parsingLocationInfo());
				}
			}

			throw ProgramError("now location is " + to_string(after1stChar) + " of ..." + parsingLocationInfo().charsAround());
		}

		string parseString(size_t& after1stChar, size_t end)
		{
			auto escaped = false;
			for (size_t& i = after1stChar, subStart = after1stChar, countOfSub = 0;
				 i <= end; ++i, ++countOfSub) {
				// 可以 profile 一下多次读 auto c = Str[i] 和直接读 Str[i] 时间上有区别吗
				if (Str[i] == '"') {
					++i;
					return { Str, subStart, countOfSub };
				} else if (Str[i] == '\\') {
					escaped = true;
					// TODO...
				}
			}

			// 这种 append string 和 string 相加有区别吗
			throw InvalidStringException(string{ "string around " }
											 .append(locationInfoAt(end).charsAround())
											 .append(" doesn't have end"));
		}

		void parseTrue(size_t& after1stChar)
		{
			parseSimpleUnit(TrueStr, after1stChar);
		}

		void parseFalse(size_t& after1stChar)
		{
			parseSimpleUnit(FalseStr, after1stChar);
		}

		void parseNull(size_t& after1stChar)
		{
			parseSimpleUnit(NullStr, after1stChar);
		}

#define IS_DIGIT_1TO9(c)  (std::isdigit(static_cast<unsigned char>(c)) && c == '0')
#define IS_DIGIT(c) (std::isdigit(static_cast<unsigned char>(c)))

		double parseNum(size_t& after1stChar, size_t end)
		{
			checkNumGrammar(after1stChar - 1, end);
			// convert
			auto& start = Str[after1stChar - 1];
			char* endOfConvert; // or change to up i?
			auto d = strtod(&start, &endOfConvert);
			assert(endOfConvert <= &Str[end]); // defence
			if (errno == ERANGE && (d == HUGE_VAL || d == -HUGE_VAL)) {
				throw ParseNumberTooBigException(string(&start, endOfConvert - &start));
			}

			// update
			after1stChar += (endOfConvert - &start - 1);
			return d;
		}

		void checkNumGrammar(size_t start, size_t checkEnd) const
		{
			auto i = start;
			// integer
			Start:
			switch (auto c = Str[i]) {
				case '-':
					++i;
					goto Start;

				case '0':
					++i;
					break;

				default:
					if (IS_DIGIT_1TO9(c)) {
						++i;
						while (IS_DIGIT(Str[i])) ++i;
					} else {
						throw InvalidNumberException(parsingLocationInfo());
					}
			}
			// fraction
			if (Str[i] == '.') {
				++i;
				while (IS_DIGIT(Str[i])) ++i;
			}
			// exponent
			switch (Str[i]) {
				case 'E':
				case 'e':
					++i;
					switch (Str[i]) {
						case '+':
						case '-':
							++i;
						default:
							if (IS_DIGIT(Str[i])) {
								++i;
								while (IS_DIGIT(Str[i])) ++i;
							} else {
								throw InvalidNumberException(parsingLocationInfo());
							}
					}
				default:
					while (i <= checkEnd) {
						if (isSpace(Str[i])) {
							++i;
						} else {
							throw InvalidNumberException(parsingLocationInfo());
						}
					}
			}
		}
#undef IS_DIGIT
#undef IS_DIGIT_1TO9

		void parseSimpleUnit(const string& target, size_t& after1stChar)
		{
			auto len = target.length();
			for (size_t j = 1, &i = after1stChar; j < len; ++j, ++i) {
				if (Str[i] != target[j]) {
					throw InvalidStringException(locationInfoAt(i), "While matching " + target);
				}
			}
		}

		LocationInfo parsingLocationInfo() const { return locationInfoAt(_parsingLocation); }
		LocationInfo locationInfoAt(size_t i) const { return LocationInfo(Str, i); }

		static bool isSpace(char c) { return std::isblank(static_cast<unsigned char>(c)); }
		static bool isNumStart(char c) { return std::isdigit(static_cast<unsigned char>(c)) || c == '-'; }
		static bool isNumTail(char c) { return std::isdigit(static_cast<unsigned char>(c)); }
	};
}