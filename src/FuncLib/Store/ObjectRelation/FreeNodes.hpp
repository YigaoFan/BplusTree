#pragma once
#include "LabelTree.hpp"

namespace FuncLib::Store::ObjectRelation
{
	class FreeNodes : private LabelTree
	{
	private:
		using Base = LabelTree;
	public:
		using Base::Base;
		using Base::AddSub;
		using Base::Take;

		void ReleaseAll(auto const& releaser)
		{
			_fileRoot.ReleaseAll(releaser);
		}
	};	
}
