#pragma once
#include <vector>
#include "Store/StaticConfig.hpp"

namespace FuncLib
{
	using FuncLib::Store::pos_int;
	using ::std::vector;

	class FuncBodyLib
	{
	private:
		/* data */
	public:
		FuncBodyLib();
		pos_int Add(vector<char> const& funcBytes)
		{

		}

		// body 的大小存在 lib 文件里吧？
		vector<char> Read(pos_int pos, size_t size)
		{

		}
		
	};
	
	FuncBodyLib::FuncBodyLib()
	{
	}
}
