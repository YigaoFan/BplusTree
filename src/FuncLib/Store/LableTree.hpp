#pragma once
#include "LableRelationNode.hpp"

namespace FuncLib::Store
{
	class LableTree
	{
	protected:
		LableRelationNode _fileRoot{ FileLable };
	public:
		LableTree();
		LableTree(LableRelationNode root);
		optional<LableRelationNode> Take(pos_lable lable);
		void AddSub(LableRelationNode notUsedNode);
	};
}
