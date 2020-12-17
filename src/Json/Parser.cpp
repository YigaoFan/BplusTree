#include <string>
#include <vector>
#include <cctype>
#include <cstdlib>
#include <cerrno>
#include <cmath>
#include <string_view>
#include "ParseException.hpp"
#include "Json.hpp"
#include "../Basic/Debug.hpp"
#include "LocationInfo.hpp"
#include "Parser.hpp"

namespace Json
{
	using ::std::strtod;
	using ::std::move;
	using ::Basic::Assert;

	JsonObject Parse(string_view jsonStr)
	{
		return Parser(jsonStr).DoParse();
	}

	Parser::Parser(string_view str) : Str(str), _len(str.length()), _currentIndex(0)
	{ }

	JsonObject Parser::DoParse()
	{
		auto j = ParseForwardUnit(DetectForwardUnitType());
		CheckSingleRoot(_currentIndex);
		return move(j);
	}

	JsonObject Parser::ParseNest()
	{
		return ParseForwardUnit(DetectForwardUnitType());
	}

	JsonObject Parser::ParseForwardUnit(JsonType type)
	{
		// TODO parse method below should ensure i is index after last char after parse
		switch (type)
		{
		case JsonType::Object:
			return JsonObject(ParseObject());
		case JsonType::Array:
			return JsonObject(ParseArray());
		case JsonType::Number:
			return JsonObject(ParseNum());
		case JsonType::String:
			return JsonObject(ParseString());
		case JsonType::True:
			return JsonObject(ParseTrue());
		case JsonType::False:
			return JsonObject(ParseFalse());
		case JsonType::Null:
			ParseNull();
			return JsonObject();
		}

		throw ProgramError("Encounter unhandled JsonType in ParseForwardUnit");
	}

	void Parser::CheckSingleRoot(size_t i)
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
	JsonType Parser::DetectForwardUnitType()
	{
		for (auto& i = _currentIndex; i < _len;)
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

		// TODO use i but not add 1 and check once, should use _currentIndex
	JsonObject::_Object Parser::ParseObject()
	{
		auto& i = _currentIndex;
		JsonObject::_Object objectMap;
		auto expectString = true, expectBracket = true, expectColon = false,
			expectJson = false, expectComma = false;
		string key;

		while (i < _len)
		{
			auto c = Str[i];
			if (IsSpace(c))
			{
				++i;
				continue;
			}
			else if (expectString && c == '"')
			{
				++i;
				key = ParseString();
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
				objectMap.emplace(move(key), ParseNest());
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

		// TODO locate info should perfect
		throw InvalidStringException(CurrentLocationInfo(), "Object not end on suitable location");
	}

	JsonObject::_Array Parser::ParseArray()
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
				array.emplace_back(ParseNest());
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

	string Parser::ParseString()
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

	double Parser::ParseNum()
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

#define INC_THEN_RETURN_IF_END if (++i == _len) { return; }
#define INC_THEN_EXCEPTION_IF_END if (++i == _len) { throw InvalidNumberException(LocationInfoAt(i)); }
	void Parser::CheckNumGrammar(size_t start) const
	{
		auto i = start;
	Start:
		switch (auto c = Str[i])
		{
		case '-':
			INC_THEN_EXCEPTION_IF_END;
			goto Start;
		case '0':
			INC_THEN_RETURN_IF_END;
			break;
		default:
			if (IS_DIGIT_1TO9(c))
			{
				INC_THEN_RETURN_IF_END;
				while (IS_DIGIT(Str[i])) { INC_THEN_RETURN_IF_END; }
			}
			else
			{
				throw InvalidNumberException(LocationInfoAt(i));
			}
		}

		// Fraction
		if (Str[i] == '.')
		{
			INC_THEN_EXCEPTION_IF_END;
			if (IS_DIGIT(Str[i]))
			{
				INC_THEN_RETURN_IF_END;
				while (IS_DIGIT(Str[i])) { INC_THEN_RETURN_IF_END; }
			}
			else
			{
				throw InvalidNumberException(LocationInfoAt(i));
			}
		}
		else
		{
			return;
		}

		// Exponent
		switch (Str[i]) 
		{
		case 'E':
		case 'e':
			INC_THEN_EXCEPTION_IF_END;
			switch (Str[i])
			{
			case '+':
			case '-':
				INC_THEN_EXCEPTION_IF_END;
			default:
				if (IS_DIGIT(Str[i]))
				{
					INC_THEN_RETURN_IF_END;
					while (IS_DIGIT(Str[i])) { INC_THEN_RETURN_IF_END; }
				}
				else
				{
					throw InvalidNumberException(LocationInfoAt(i));
				}
			}

		default:
			return;
		}
	}
#undef INC_THEN_EXCEPTION_IF_END
#undef INC_THEN_RETURN_IF_END
#undef IS_DIGIT
#undef IS_DIGIT_1TO9

	bool Parser::ParseTrue()
	{
		ParseSimpleUnit(True);
		return true;
	}

	bool Parser::ParseFalse()
	{
		ParseSimpleUnit(False);
		return false;
	}

	void Parser::ParseNull()
	{
		ParseSimpleUnit(Null);
	}

	void Parser::ParseSimpleUnit(string_view target)
	{
		for (size_t j = 1, &i = _currentIndex; j < target.length(); ++j, ++i)
		{
			if (Str[i] != target[j])
			{
				throw InvalidStringException(LocationInfoAt(i), string("While matching ") + string(target));
			}
		}
	}

	LocationInfo Parser::CurrentLocationInfo() const { return LocationInfoAt(_currentIndex); }
	LocationInfo Parser::LocationInfoAt(size_t i) const { return LocationInfo(Str, i); }
	bool Parser::IsSpace(char c) { return ::std::isspace(static_cast<unsigned char>(c)); }
	bool Parser::IsNumStart(char c) { return ::std::isdigit(static_cast<unsigned char>(c)) || c == '-'; }
}