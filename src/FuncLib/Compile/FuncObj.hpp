#pragma once
#include <vector>
#include <string>
#include "FuncType.hpp"

namespace FuncLib::Compile
{
	using ::std::string;
	using ::std::vector;

	class FuncObj
	{
	private:
		FuncType _type;
		string _summary;
		vector<char> _bodyBytes;

	public:
		FuncObj(/* args */);

		FuncType& Type()
		{
			return _type;
		}
		
		string& Summary()
		{
			return _summary;
		}

		vector<char>& BodyBytes()
		{
			return _bodyBytes;
		}
	};
	
	FuncObj::FuncObj(/* args */)
	{
	}		
}
