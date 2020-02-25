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
		class Index
		{
		private:
			size_t _i = 0;
			size_t _len;

		public:
			Index(size_t len) : _len(len) { }
			size_t operator++ ()
			{
				if (++_i < _len)
				{
					return _i;
				}

				throw AccessOutOfRangeException();
			}

			operator size_t()
			{
				return _i;
			}
		};
		
		friend JsonObject Parse(string_view);
		string_view True = "true";
		string_view False = "false";
		string_view Null = "null";
		string_view Str;
		// Index _currentIndex;
		size_t _currentLocation = 0;
		size_t _len;

		Parser(string_view str) : Str(str), _len(str.length())
		{ }

		JsonObject DoParse()
		{
			auto j = ParseForwardUnit(DetectForwardUnitType(_currentLocation), _currentLocation);
			CheckSingleRoot(_currentLocation);
			return move(j);
		}

		JsonObject ParseNest()
		{
			// _currentLocation is index after 1st char
			return ParseForwardUnit(DetectForwardUnitType(_currentLocation), _currentLocation);
		}

		JsonObject ParseForwardUnit(JsonType type, size_t& indexAfter1stChar)
		{
			// TODO parse method below should ensure i is index after last char after parse
			switch (type)
			{
			case JsonType::Object:
				return JsonObject(ParseObject(indexAfter1stChar));
			case JsonType::Array:
				return JsonObject(ParseArray(indexAfter1stChar));
			case JsonType::Number:
				return JsonObject(ParseNum(indexAfter1stChar));
			case JsonType::String:
				return JsonObject(ParseString(indexAfter1stChar));
			case JsonType::True:
				return JsonObject(ParseTrue(indexAfter1stChar));
			case JsonType::False:
				return JsonObject(ParseFalse(indexAfter1stChar));
			case JsonType::Null:
				ParseNull(indexAfter1stChar);
				return JsonObject();
			}

			throw ProgramError("Encounter unknown JsonType in ParseForwardUnit");
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
		JsonType DetectForwardUnitType(size_t& start)
		{
			for (auto& i = start; i < _len;)
			{
				auto c = Str[i++];
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

		JsonObject::_Object ParseObject(size_t& indexAfter1stChar)
		{
			JsonObject::_Object objectMap;
			auto expectString = true, expectBracket = true, expectColon = false,
				expectJson = false, expectComma = false;
			string key;

			for (auto& i = indexAfter1stChar; i < _len;)
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

			throw ProgramError("now location is " + to_string(indexAfter1stChar) + " of ..." + CurrentLocationInfo().StringAround());
		}

		JsonObject::_Array ParseArray(size_t& indexAfter1stChar)
		{
			JsonObject::_Array array;
			auto expectJson = true, expectSqrBracket = true, expectComma = false;

			for (auto& i = indexAfter1stChar; i < _len;)
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

			throw ProgramError("now location is " + to_string(indexAfter1stChar) + " of ..." + CurrentLocationInfo().StringAround());
		}

		string ParseString(size_t& indexAfter1stChar)
		{
			auto escaped = false;
			auto start = indexAfter1stChar;
			for (size_t& i = indexAfter1stChar; i < _len; ++i)
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

			throw InvalidStringException(string{ "string " } + LocationInfoAt(_len).StringAround()
											 + (" doesn't have right end"));
		}

#define IS_DIGIT_1TO9(c)  (std::isdigit(static_cast<unsigned char>(c)) && c != '0')
#define IS_DIGIT(c) (std::isdigit(static_cast<unsigned char>(c)))

		double ParseNum(size_t& indexAfter1stChar)
		{
			CheckNumGrammar(indexAfter1stChar - 1);
			// Convert
			auto start = &Str[indexAfter1stChar - 1];
			char* endOfConvert; // or change to up i?
			auto num = strtod(start, &endOfConvert);
			if (errno == ERANGE && (num == HUGE_VAL || num == -HUGE_VAL))
			{
				throw ParseNumberTooBigException(string(start, endOfConvert - start));
			}

			indexAfter1stChar += (endOfConvert - start - 1);
			return num;
		}

		void CheckNumGrammar(size_t start) const
		{
			auto i = start;
			// Integer
			// TODO ++i maybe out of range all ++i, like while(++i)...
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
				while (IS_DIGIT(Str[i])) { ++i; }
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
						throw InvalidNumberException(CurrentLocationInfo());
					}
				}

				UNIT_END_CHECK;
				// TODO check the correct ending, other unit parse is same
			default:
				throw InvalidNumberException(CurrentLocationInfo());
			}
		}
#undef UNIT_END_CHECK
#undef IS_DIGIT
#undef IS_DIGIT_1TO9

		bool ParseTrue(size_t& indexAfter1stChar)
		{
			ParseSimpleUnit(True, indexAfter1stChar);
			return true;
		}

		bool ParseFalse(size_t& indexAfter1stChar)
		{
			ParseSimpleUnit(False, indexAfter1stChar);
			return false;
		}

		void ParseNull(size_t& indexAfter1stChar)
		{
			ParseSimpleUnit(Null, indexAfter1stChar);
		}
		
		void ParseSimpleUnit(string_view target, size_t& indexAfter1stChar)
		{
			for (size_t j = 1, &i = indexAfter1stChar; j < target.length(); ++j, ++i)
			{
				if (Str[i] != target[j])
				{
					throw InvalidStringException(LocationInfoAt(i), string("While matching ") + string(target));
				}
			}

			++indexAfter1stChar;
		}

		LocationInfo CurrentLocationInfo() const { return LocationInfoAt(_currentLocation); }
		LocationInfo LocationInfoAt(size_t i) const { return LocationInfo(Str, i); }
		static bool IsSpace(char c) { return std::isblank(static_cast<unsigned char>(c)); }
		static bool IsNumStart(char c) { return std::isdigit(static_cast<unsigned char>(c)) || c == '-'; }
		static bool IsNumTail(char c) { return std::isdigit(static_cast<unsigned char>(c)); }
	};
}