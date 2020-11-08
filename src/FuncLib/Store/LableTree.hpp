#pragma once
#include "LableRelationNode.hpp"

namespace FuncLib::Store
{
	class LableTree
	{
	protected:
		LableRelationNode _fileRoot{ FileLable };
	public:
		optional<LableRelationNode> Take(pos_lable lable)
		{
			// because lable cannot equal to FileLable, so take internal directly
			return _fileRoot.TakeInternal(lable);
		}

		void AddSub(LableRelationNode notUsedNode)
		{
			_fileRoot.AddSub(move(notUsedNode));
		}
	};
}
