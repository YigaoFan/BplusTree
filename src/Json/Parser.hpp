#pragma once
#include <string>
#include <string_view>
#include "Json.hpp"
#include "LocationInfo.hpp"

namespace Json 
{
	using ::std::string;
	using ::std::size_t;
	using ::std::string_view;

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

		Parser(string_view str);
		JsonObject DoParse();
		JsonObject ParseNest();
		JsonObject ParseForwardUnit(JsonType type);

		void CheckSingleRoot(size_t i);
		JsonType DetectForwardUnitType();
		JsonObject::_Object ParseObject();
		JsonObject::_Array ParseArray();
		string ParseString();
		double ParseNum();
		void CheckNumGrammar(size_t start) const;
		bool ParseTrue();
		bool ParseFalse();
		void ParseNull();
		void ParseSimpleUnit(string_view target);

		LocationInfo CurrentLocationInfo() const;
		LocationInfo LocationInfoAt(size_t i) const;
		static bool IsSpace(char c);
		static bool IsNumStart(char c);
	};
}