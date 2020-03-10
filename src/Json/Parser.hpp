#pragma once
#include <string>
#include <vector>
#include <cctype>
#include <memory>
#include <cstdlib>
#include <cerrno>
#include <cmath>
#include <string_view>
#include "ParseException.hpp"
#include "Json.hpp"
#include "../Basic/Debug.hpp"
#include "../Basic/Exception.hpp"
#include "LocationInfo.hpp"

namespace Json 
{
	using ::std::string;
	using ::std::make_shared;
	using ::std::size_t;
	using ::std::strtod;
	using ::std::string_view;
	using ::std::move;
	using ::Debug::Assert;
	using ::Basic::AccessOutOfRangeException;

	JsonObject Parse(string_view jsonStr);

	class Parser 
	{
	private:
		friend JsonObject Parse(string_view);
		string_view True = "true";
		string_view False = "false";
		string_view Null = "null";
		string_view Str;
		size_t _len;
		size_t _currentIndex;

		Parser(string_view str) : Str(str), _len(str.length()), _currentIndex(0)
		{ }

		JsonObject DoParse()
		{
			auto j = ParseForwardUnit(DetectForwardUnitType());
			CheckSingleRoot(_currentIndex);
			return move(j);
		}

		JsonObject ParseNest()
		{
			return ParseForwardUnit(DetectForwardUnitType());
		}

		JsonObject ParseForwardUnit(JsonType type)
		{
			// TODO parse method below should ensure i is index after last char after parse
			switch (type)
			{
			case JsonType::Object:
				return ParseObject();
			case JsonType::Array:
				return ParseArray();
			case JsonType::Number:
				return ParseNum();
			case JsonType::String:
				return ParseString();
			case JsonType::True:
				return ParseTrue();
			case JsonType::False:
				return ParseFalse();
			case JsonType::Null:
				ParseNull();
				return JsonObject();
			}

			throw ProgramError("Encounter unhandled JsonType in ParseForwardUnit");
		}

		void CheckSingleRoot(size_t i)
		{
			for (; i < _len; ++i)
			{
				if (!IsSpace(Str[i]))
				{
					throw ParseNotSingleRootException(LocationInfoAt(i));
				}
			}
		}

		/// Detect forward unit parse type and move start to next position
		JsonType DetectForwardUnitType()
		{
			auto& start = _currentIndex;
			for (auto& i = start; i < _len; ++i)
			{
				auto c = Str[i];
				switch (c) 
				{
				case '{': return JsonType::Object;
				case '[': return JsonType::Array;
				case '"': return JsonType::String;
				case 't': return JsonType::True;
				case 'f': return JsonType::False;
				case 'n': return JsonType::Null;
				default:
					if (IsSpace(c))
					{
						continue;
					}
					else if (IsNumStart(c))
					{
						return JsonType::Number;
					}
					else
					{
						throw InvalidStringException(CurrentLocationInfo());
					}
				}
			}

			throw ParseExpectValueException();
		}

		// TODO use i but not add 1 and check once, should use _currentIndex
		JsonObject::_Object ParseObject()
		{
			JsonObject::_Object objectMap;
			auto expectString = true, expectBracket = true, expectColon = false,
				expectJson = false, expectComma = false;
			string key;

			for (auto& i = _currentIndex; i < _len;)
			{
				auto c = Str[i];
				if (IsSpace(c))
				{
					++i;
					continue;
				}
				else if (expectString && c == '"')
				{
					key = ParseString(++i);
					expectString = expectBracket = false;
					expectColon = true;
				}
				else if (expectColon && c == ':')
				{
					++i;
					expectColon = false;
					expectJson = true;
				}
				else if (expectJson)
				{
					objectMap.emplace(move(key), make_shared<JsonObject>(ParseNest()));
					key.clear();
					expectJson = false;
					expectComma = expectBracket = true;
				}
				else if (expectComma && c == ',')
				{
					// Support ',' as end of object
					++i;
					expectComma = false;
					expectString = expectBracket = true;
				}
				else if (expectBracket && c == '}')
				{
					++i;
					return objectMap;
				}
				else
				{
					throw InvalidStringException(CurrentLocationInfo());
				}
			}

			throw InvalidStringException(CurrentLocationInfo(), "Object not end on suitable location");
		}

		JsonObject::_Array ParseArray()
		{
			auto& i = _currentIndex;
			JsonObject::_Array array;
			auto expectJson = true, expectSqrBracket = true, expectComma = false;

			while (i < _len)
			{
				auto c = Str[i];
				if (IsSpace(c))
				{
					++i;
					continue;
				}
				else if (expectSqrBracket && c == ']')
				{
					++i;
					return move(array);
				}
				else if (expectComma && c == ',')
				{
					// support ',' as end of array
					++i;
					expectComma = false;
					expectJson = expectSqrBracket = true;
				}
				else if (expectJson)
				{
					array.emplace_back(make_shared<JsonObject>(ParseNest()));
					expectJson = false;
					expectComma = expectSqrBracket = true;
				}
				else
				{
					throw InvalidStringException(CurrentLocationInfo());
				}
			}

			throw InvalidStringException(CurrentLocationInfo(), "Array not end on suitable location");
		}

		string ParseString()
		{
			auto escaped = false;
			auto start = _currentIndex;
			for (auto& i = _currentIndex; i < _len; ++i)
			{
				if (Str[i] == '"')
				{
					// TODO check below cons right
					return { Str, start, (i++) - start };
				}
				else if (Str[i] == '\\')
				{
					escaped = true;
					// TODO...
				}
			}

			throw InvalidStringException("string doesn't have right end");
		}

#define IS_DIGIT_1TO9(c)  (::std::isdigit(static_cast<unsigned char>(c)) && c != '0')
#define IS_DIGIT(c) (::std::isdigit(static_cast<unsigned char>(c)))

		double ParseNum()
		{
			auto& i = _currentIndex;
			CheckNumGrammar(i - 1);
			// Convert
			auto start = &Str[i - 1];
			char* numEnd;
			auto num = strtod(start, &numEnd);
			if (errno == ERANGE && (num == HUGE_VAL || num == -HUGE_VAL))
			{
				errno = 0;
				throw ParseNumberTooBigException(string(start, numEnd - start));
			}

			i += (numEnd - start - 1);
			return num;
		}

		void CheckNumGrammar(size_t start) const
		{
			auto i = start;
			// Integer
			// TODO ++i maybe out of range all ++i
			// while(++i) maybe access end of string_view?
			// TODO Some point could end
		Start:
			switch (auto c = Str[i])
			{
			case '-':
				++i;
				goto Start;
			case '0':
				++i;
				break;
			default:
				if (IS_DIGIT_1TO9(c))
				{
					++i;
					while (IS_DIGIT(Str[i])) { ++i; }
				}
				else
				{
					throw InvalidNumberException(CurrentLocationInfo());
				}
			}

#define UNIT_END_CHECK                \
	if (i == _len || IsSpace(Str[i])) \
	{                                 \
		return;                       \
	}

			UNIT_END_CHECK;

			// Fraction
			if (Str[i] == '.')
			{
				++i;
				if (IS_DIGIT(Str[i]))
				{
					++i;
					while (IS_DIGIT(Str[i])) { ++i; }
				}
				else
				{
					throw InvalidNumberException(LocationInfoAt(i));
				}
			}

			UNIT_END_CHECK;

			// Exponent
			switch (Str[i]) 
			{
			case 'E':
			case 'e':
				++i;
				switch (Str[i])
				{
				case '+':
				case '-':
					++i;
				default:
					if (IS_DIGIT(Str[i]))
					{
						++i;
						while (IS_DIGIT(Str[i])) { ++i; }
					}
					else
					{
						throw InvalidNumberException(LocationInfoAt(i));
					}
				}

				UNIT_END_CHECK;
			default:
				throw InvalidNumberException(LocationInfoAt(i));
			}
		}
#undef UNIT_END_CHECK
#undef IS_DIGIT
#undef IS_DIGIT_1TO9

		bool ParseTrue()
		{
			ParseSimpleUnit(True);
			return true;
		}

		bool ParseFalse()
		{
			ParseSimpleUnit(False);
			return false;
		}

		void ParseNull()
		{
			ParseSimpleUnit(Null);
		}
		/// \0 end require
		void ParseSimpleUnit(string_view target)
		{
			for (size_t j = 1, &i = _currentIndex; j < target.length(); ++j, ++i)
			{
				if (Str[i] != target[j])
				{
					throw InvalidStringException(LocationInfoAt(i), string("While matching ") + string(target));
				}
			}
		}

		LocationInfo CurrentLocationInfo() const { return LocationInfoAt(_currentIndex); }
		LocationInfo LocationInfoAt(size_t i) const { return LocationInfo(Str, i); }
		static bool IsSpace(char c) { return std::isblank(static_cast<unsigned char>(c)); }
		static bool IsNumStart(char c) { return std::isdigit(static_cast<unsigned char>(c)) || c == '-'; }
		static bool IsNumTail(char c) { return std::isdigit(static_cast<unsigned char>(c)); }
	};
}