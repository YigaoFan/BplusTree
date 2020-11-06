#include "LableRelationNode.hpp"

namespace FuncLib::Store
{
	LableRelationNode::LableRelationNode(pos_lable lable) : _lable(lable)
	{ }

	LableRelationNode::LableRelationNode(pos_lable lable, vector<LableRelationNode> subNodes)
		: _lable(lable), _subNodes(move(subNodes))
	{ }

	void LableRelationNode::Subs(vector<LableRelationNode> subNodes)
	{
		_subNodes = move(subNodes);
	}

	pos_lable LableRelationNode::Lable() const
	{
		return _lable;
	}
}
