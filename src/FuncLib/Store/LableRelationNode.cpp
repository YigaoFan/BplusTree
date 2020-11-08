#include "LableRelationNode.hpp"

namespace FuncLib::Store
{
	LableRelationNode::LableRelationNode(pos_lable lable) : _lable(lable)
	{ }

	LableRelationNode::LableRelationNode(pos_lable lable, vector<LableRelationNode> subNodes)
		: _lable(lable), _subNodes(move(subNodes))
	{ }

	/// 这里要保证 subs 的 lable 升序排列
	void LableRelationNode::Subs(vector<LableRelationNode> subNodes)
	{
		_subNodes = move(subNodes);
	}

	void LableRelationNode::AddSub(LableRelationNode node)
	{
		for (auto it = _subNodes.begin(); it != _subNodes.end(); ++it)
		{
			if (node.Lable() <= it->Lable())
			{
				_subNodes.insert(it, move(node));
			}
		}

		_subNodes.push_back(move(node));
	}

	pos_lable LableRelationNode::Lable() const
	{
		return _lable;
	}
}
