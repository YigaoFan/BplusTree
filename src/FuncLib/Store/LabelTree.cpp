#include "LabelTree.hpp"

namespace FuncLib::Store
{
	using ::std::move;

	LabelTree::LabelTree() : _fileRoot(FileLabel) {}
	LabelTree::LabelTree(LabelRelationNode root) : _fileRoot(move(root)) {}

	optional<LabelRelationNode> LabelTree::Take(pos_label label)
	{
		// because label cannot equal to FileLabel, so take internal directly
		return _fileRoot.TakeInside(label);
	}

	void LabelTree::AddSub(LabelRelationNode node)
	{
		_fileRoot.AddSub(move(node));
	}
}
