#pragma once
#include "LabelNode.hpp"

namespace FuncLib::Store
{
	class LabelTree
	{
	protected:
		LabelNode _fileRoot{ FileLabel };
	public:
		LabelTree();
		LabelTree(LabelNode root);
		optional<LabelNode> Take(pos_label label);
		void AddSub(LabelNode node);
		bool EqualTo(LabelTree const& that) const;
	};
}
