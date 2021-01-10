#include "LabelTree.hpp"

namespace FuncLib::Store::ObjectRelation
{
	using ::std::move;

	LabelTree::LabelTree() : _fileRoot(FileLabel) {}
	LabelTree::LabelTree(LabelNode root) : _fileRoot(move(root)) {}

	optional<LabelNode> LabelTree::Take(pos_label label)
	{
		// because label cannot equal to FileLabel, so take internal directly
		return _fileRoot.TakeInside(label);
	}

	void LabelTree::AddSub(LabelNode node)
	{
		_fileRoot.AddSub(move(node));
	}

	bool LabelTree::EqualTo(LabelTree const& that) const
	{
		return _fileRoot.EqualTo(that._fileRoot);
	}
}
