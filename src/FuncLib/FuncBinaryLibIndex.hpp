#pragma once
#include <utility>
#include "Store/StaticConfig.hpp"
#include "Compile/FuncObj.hpp"
#include "Compile/FuncType.hpp"

namespace FuncLib
{
	using FuncLib::Compile::FuncObj;
	using FuncLib::Compile::FuncType;
	using FuncLib::Store::pos_int;
	using ::std::pair;

	class FuncBinaryLibIndex
	{
	private:
		/* data */
	public:
		FuncBinaryLibIndex(/* args */);
		void Add(pos_int pos, FuncObj funcObj)
		{

		}

		pair<pos_int, size_t> GetStoreInfo(FuncType const& type) const
		{

		}

		bool Contains(FuncType const& type) const
		{
			return false;
		}

		void ModifyFuncName(string funcName, string newFuncName)
		{
		}
	};
	
	FuncBinaryLibIndex::FuncBinaryLibIndex(/* args */)
	{
	}
}
