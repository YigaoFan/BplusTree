#pragma once
#include <string>
#include <vector>
#include <cctype>
#include "Json.hpp"

namespace Json {
	using std::string;

	class Parser {
	private:
		string _str;

		Parser(string str)
			: _str(std::move(str))
		{ }

		void
		removeWhitespaces()
		{
			vector<uint16_t> toBeRemoved; // index included
			for (auto& c : _str) {
				// TODO convert char to int?
				if (isSpace(c) && !inJsonString(/*get index in string*/)) {
					toBeRemoved.push_back(/*index*/)
				}
			}

			for (auto c : toBeRemoved) {
				removeChar(index);
			}
		}

		void
		removeChar(uint16_t i)
		{
			// how to remove a char in string
		}

		Json
		doParse()
		{
			
		}
		
		static
		bool
		isSpace(char c)
		{
			 // check if work on which char
			return std::isblank(static_cast<unsigned char>(c));
		}

		static
		bool
		inJsonString(uint16_t i)
		{
			// search around
		}
	public:
		static
		Json
		parse(string str)
		{
			auto p = Parser(std::move(str));
			p.removeWhitespaces().doParse();
		}
	};

	// use namespace to hide some functions
	namespace {

	}
}