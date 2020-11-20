#include "LabelRelationNode.hpp"

namespace FuncLib::Store
{
	LabelRelationNode::LabelRelationNode(pos_label label) : _label(label)
	{ }

	LabelRelationNode::LabelRelationNode(pos_label label, vector<LabelRelationNode> subNodes)
		: _label(label), _subNodes(move(subNodes))
	{ }

	/// 这里要保证 subs 的 label 升序排列
	void LabelRelationNode::Subs(vector<LabelRelationNode> subNodes)
	{
		_subNodes = move(subNodes);
	}

	void LabelRelationNode::AddSub(LabelRelationNode node)
	{
		for (auto it = _subNodes.begin(); it != _subNodes.end(); ++it)
		{
			if (node.Label() <= it->Label())
			{
				_subNodes.insert(it, move(node));
			}
		}

		_subNodes.push_back(move(node));
	}

	pos_label LabelRelationNode::Label() const
	{
		return _label;
	}
}
