#pragma once
#include <string>
#include <vector>
#include <cctype>
#include <memory>
#include <utility>
#include "ParseException.hpp"
#include "Json.hpp"
#include "LocationInfo.hpp"

namespace Json {
	using std::string;
	using std::make_shared;
	using std::pair;
	using std::make_pair;

	class Parser {
	public:
		static Json parse(const string& jsonStr)
		{
			return Parser(jsonStr).doParse();
		}

		explicit Parser(const string& str) : Str(str)
		{ }

		/**
		 * should ensure the string to be parsed exists in current scope
		 */
		inline
		Json
		doParse()
		{
			_parsingLocation = 0;
			if (auto len = Str.length()) {
				return parseJson(_parsingLocation, len - 1);
			}

			throw ParseEmptyStringException();
		}

	private:
		const string TrueStr = "true";
		const string FalseStr = "false";
		const string NullStr = "null";
		const string& Str;
		size_t _parsingLocation = 0;

		inline
		LocationInfo
		parsingLocationInfo() const
		{
			return locationInfoAt(_parsingLocation);
		}

		inline
		LocationInfo
		locationInfoAt(size_t i) const
		{
			return LocationInfo(Str, i);
		}

		Json
		parseJson(size_t& start, size_t end)
		{
			auto typeAndRangeRightPair = routeRootParse(start, end);
			return rootParseByType(typeAndRangeRightPair.first, start, typeAndRangeRightPair.second);
		}

		inline
		Json
		rootParseByType(Json::Type parseType, size_t& start, size_t end)
		{
			switch (parseType) {
				case Json::Type::Object:
					return Json(Object(), parseObject(start, end));

				case Json::Type::Array:
					return Json(Array(), parseArray(start, end));

				case Json::Type::Number:
					return Json(Number(), parseNum(start, e));

				case Json::Type::String:
					return Json(String(), parseString(start));

				case Json::Type::True:
					parseTrue(/* TODO */);
					return Json(True());

				case Json::Type::False:
					parseFalse(/* TODO */);
					return Json(False());

				case Json::Type::Null:
					parseNull(/* TODO */);
					return Json(Null());
			}
		}

		/**
		 * change rangeLeftToChange to right start position of corresponding type
		 * @return pair of Json type and range right roughly from external eye
		 */
		// 所有的位置应该是偏向类型内的，比如 Object 的位置包含}，Number 都在 Number 内部上
		pair<Json::Type, size_t>
		routeRootParse(size_t& rangeLeftToChange, size_t rangeEnd)
		{
			auto type = detectForwardUnitType(rangeLeftToChange, rangeEnd);
			size_t& i = rangeLeftToChange, rangeRight;
			switch (type) {
				case Json::Type::Object:
					rangeRight = findRightPair(i, rangeEnd, '}');
					break;

				case Json::Type::Array:
					rangeRight = findRightPair(i, rangeEnd, ']');
					break;

				case Json::Type::String:
					rangeRight = findRightPair(i, rangeEnd, '"');
					break;

				case Json::Type::True:
					rangeRight = i + 2; // rue
					ensureSingleRoot(rangeRight+1, rangeEnd);
					break;

				case Json::Type::False:
					rangeRight = i + 3; // alse
					ensureSingleRoot(rangeRight+1, rangeEnd);
					break;

				case Json::Type::Null:
					rangeRight = i + 2; // ull
					ensureSingleRoot(rangeRight+1, rangeEnd);
					break;

				case Json::Type::Number:
					rangeRight = findNumStrRightBound(i+1, rangeEnd);
					break;
			}

			return make_pair(type, rangeRight);
		}

		inline
		static
		bool
		isSpace(char c)
		{
			return std::isblank(static_cast<unsigned char>(c));
		}

		inline
		static
		bool
		isNumStart(char c)
		{
			return std::isdigit(static_cast<unsigned char>(c)) || c == '-';
		}

		inline
		static
		bool
		isNumTail(char c)
		{
			return std::isdigit(static_cast<unsigned char>(c));
		}

		bool
		inJsonString(size_t i)
		{
			// search around
		}

		/**
		 * Parse the object string without {} on both sides
		 */
		Json::_Object
		parseObject(size_t& start, size_t end)
		{
			Json::_Object objectMap;

			auto expectString = true, expectBracket = true, expectColon = false,
			     expectJson = false, expectComma = false;
			string key;
			shared_ptr<Json> value;
			for (auto& i = start; i <= end; ++i) {
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
					// 主要是这块的这个递归太伤了
					value = make_shared<Json>(forwardParseJsonUnit(i, end - 1));
					objectMap.emplace(std::move(key), std::move(value));
					key.clear(); value.reset();
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

			throw ProgramError("now location is " + to_string(start) + " of ..." + parsingLocationInfo().charsAround());
		}

		Json::_Array
		parseArray(size_t& start, size_t end)
		{
			Json::_Array array;

			auto expectJson = true, expectSqrBracket = true, expectComma = false;
			for (auto& i = start; i <= end; ++i) {
				auto c = Str[i];
				if (isSpace(c)) {
					continue;
				} else if (expectSqrBracket && c == ']') {
					// 之后详细处理下嵌套解析之间的解析之后的位置变化的关系
					++i;
					return array;
				} else if (expectComma && c == ',') {
					expectComma = false;
					expectJson = expectSqrBracket = true;
				} else if (expectJson) {
					// 这里没有加一，所以这里可能是{,[，之后的处理是否正确
					// this and up object end is a too big range
					array.emplace_back(make_shared<Json>(forwardParseJsonUnit(i, end - 1)));
					expectComma = expectSqrBracket = true;
					expectJson = false;
				}
			}
		}

		Json
		forwardParseJsonUnit(size_t& start, size_t bound)
		{
			auto type = detectForwardUnitType(start, bound);
			// 可以把 routeRootParse 里的除了 detect 的代码分出来，这样递归和 root 的差别可以比较明显的看出来了
			rootParseByType(type, start, bound);
		}

		Json::Type
		detectForwardUnitType(size_t& start, size_t bound)
		{
			for (auto& i = start; i < bound; ++i) {
				auto c = Str[i++];
				switch (c) {
					case '{':
						return Json::Type::Object;

					case '[':
						return Json::Type::Array;

					case '"':
						return Json::Type::String;

					case 't':
						return Json::Type::True;

					case 'f':
						return Json::Type::False;

					case 'n':
						return Json::Type::Null;

					default:
						if (isSpace(c)) {
							continue;
						} else if (isNumStart(c)) {
							return Json::Type::Number;
						} else {
							throw InvalidStringException(parsingLocationInfo());
						}
				}
			}

			throw ParseExpectValueException();
		}

		string
		parseString(size_t& start, size_t end)
		{
			// 处理转义
			// end is ",
			for (size_t& i = start, subStart = start, countOfSub = 0;
				i <= end; ++i, ++countOfSub) {
				if (Str[i] == '"') {
					++i;
					return { Str, subStart, countOfSub };
				}
			}

			throw InvalidStringException(string{ "string around " }
			                                           .append(locationInfoAt(end).charsAround())
			                                           .append(" doesn't have end"));
		}

		inline
		void
		parseTrue(size_t& i)
		{
			parseSimpleType(TrueStr, i);
		}

		inline
		void
		parseFalse(size_t& i)
		{
			parseSimpleType(FalseStr, i);
		}

		inline
		void
		parseNull(size_t& i)
		{
			parseSimpleType(NullStr, i);
		}

		double
		parseNum(size_t& i)
		{

		}

		void
		parseSimpleType(const string& target, size_t& i)
		{
			auto len = target.length();
			// match from second char
			for (int j = 1; j < len; ++j, ++i) {
				auto t = target[j];
				auto c = Str[i];
				if (t != c) {
					throw InvalidStringException(locationInfoAt(i), "While matching " + target);
				}
			}
		}

		/**
		 * will check single root inner
		 * @return Index of expected char
		 */
		size_t
		findRightPair(size_t start, size_t end, char expected) const
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

		void
		ensureSingleRoot(size_t checkStart, size_t checkEnd) const
		{
			for (int i = checkStart; i < checkEnd; ++i) {
				if (!isSpace(Str[i])) {
					throw ParseNotSingleRootException(locationInfoAt(i));
				}
			}
		}

		size_t
		findNumStrRightBound(size_t start, size_t end) const
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
	};

	// may use namespace to hide some functions
	namespace {

	}
}