#include "LabelTree.hpp"

namespace FuncLib::Store
{
	LabelTree::LabelTree() : _fileRoot(FileLabel) {}
	LabelTree::LabelTree(LabelRelationNode root) : _fileRoot(move(root)) {}

	optional<LabelRelationNode> LabelTree::Take(pos_label label)
	{
		// because label cannot equal to FileLabel, so take internal directly
		return _fileRoot.TakeInternal(label);
	}

	void LabelTree::AddSub(LabelRelationNode notUsedNode)
	{
		_fileRoot.AddSub(move(notUsedNode));
	}
}
