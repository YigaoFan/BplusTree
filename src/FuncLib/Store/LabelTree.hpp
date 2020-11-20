#pragma once
#include "LabelRelationNode.hpp"

namespace FuncLib::Store
{
	class LabelTree
	{
	protected:
		LabelRelationNode _fileRoot{ FileLabel };
	public:
		LabelTree();
		LabelTree(LabelRelationNode root);
		optional<LabelRelationNode> Take(pos_label label);
		void AddSub(LabelRelationNode notUsedNode);
	};
}
