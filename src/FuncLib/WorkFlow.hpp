#pragma once
#include <string>
#include <vector>

namespace FuncLib
{
	using ::std::string;
	using ::std::vector;

	struct IRawFunctionReader
	{
		virtual vector<string> ReadDefinition() = 0;
		virtual void Close() = 0;
	};

	vector<string> ProcessRawFunction()
	{
		// Check if duplicate to existed function
		// add some header
		// package
	}

	path CompileCookedFunction(vector<string>)
	{

	}

	void AddToLib(path)
	{

	}
}