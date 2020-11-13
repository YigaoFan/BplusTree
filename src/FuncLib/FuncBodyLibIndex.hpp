#pragma once
#include <utility>
#include "FuncType.hpp"
#include "Store/StaticConfig.hpp"

namespace FuncLib
{
	using FuncLib::Store::pos_int;
	using ::std::pair;

	class FuncBodyLibIndex
	{
	private:
		/* data */
	public:
		FuncBodyLibIndex(/* args */);
		void Add(pos_int pos, FuncType type, string summary)
		{

		}

		pair<pos_int, size_t> GetStoreInfo(FuncType const& type) const
		{

		}
	};
	
	FuncBodyLibIndex::FuncBodyLibIndex(/* args */)
	{
	}
}
