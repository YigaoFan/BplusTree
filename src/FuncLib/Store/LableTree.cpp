#include "LableTree.hpp"

namespace FuncLib::Store
{
	LableTree::LableTree() : _fileRoot(FileLable) {}
	LableTree::LableTree(LableRelationNode root) : _fileRoot(move(root)) {}

	optional<LableRelationNode> LableTree::Take(pos_lable lable)
	{
		// because lable cannot equal to FileLable, so take internal directly
		return _fileRoot.TakeInternal(lable);
	}

	void LableTree::AddSub(LableRelationNode notUsedNode)
	{
		_fileRoot.AddSub(move(notUsedNode));
	}
}
