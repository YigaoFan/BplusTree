#include "LabelRelationNode.hpp"

namespace FuncLib::Store
{
	using ::std::move;

	LabelRelationNode::LabelRelationNode(pos_label label) : _label(label)
	{ }

	LabelRelationNode::LabelRelationNode(pos_label label, vector<LabelRelationNode> subNodes)
		: _label(label), _subNodes(move(subNodes))
	{ }

	/// 这里要保证 subs 的 label 升序排列
	void LabelRelationNode::SetSubs(vector<LabelRelationNode> subNodes)
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

	vector<LabelRelationNode> LabelRelationNode::GiveSubs()
	{
		return move(_subNodes);
	}

	bool LabelRelationNode::SubsEmpty() const noexcept
	{
		return _subNodes.empty();
	}

	optional<LabelRelationNode> LabelRelationNode::TakeInside(pos_label label)
	{
		if (_subNodes.empty())
		{
			return {};
		}

		for (size_t i = 0; auto& n : _subNodes)
		{
			if (n.Label() == label)
			{
				auto des = move(n);
				Erase(i, _subNodes);
				return des;
			}
			else if (auto r = n.TakeInside(label); r.has_value())
			{
				return r;
			}
		}

		return {};
	}
}
