#include "LableRelationNode.hpp"

namespace FuncLib::Store
{
	LableRelationNode::LableRelationNode(pos_lable lable) : _lable(lable)
	{ }

	LableRelationNode::LableRelationNode(pos_lable lable, vector<LableRelationNode*> subNodes)
		: _lable(lable), _subLables(move(subNodes))
	{ }

	LableRelationNode::~LableRelationNode()
	{
		for (auto p : _subLables)
		{
			delete p;
		}
	}	
}
