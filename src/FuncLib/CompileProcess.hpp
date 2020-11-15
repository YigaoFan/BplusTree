#pragma once
#include <string>
#include <vector>
#include "FuncObj.hpp"

namespace FuncLib
{
	using ::std::string;
	using ::std::vector;

	// add extern
	// get function def
	// get function type
	// 改名，因为 extern 不支持重载
	// scan | compile | addToLib | addToIndex
	void Scan(vector<string> const& funcDef)
	{
		// 检测是否有 new 之类的违规情况
	}

	/// include 所有必要的头文件，让外界方便使用，后期需求
	FuncObj Compile(vector<string> funcDef)
	{
		Scan(funcDef);
		// get func type
		// convert to JsonObject arg type and return type
		// call compiler to compile it，如果有编译错误怎么办？
		// read compiled file into memory
	}
}