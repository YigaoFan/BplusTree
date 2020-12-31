#include "LabelNode.hpp"

namespace FuncLib::Store
{
	using ::std::move;

	LabelNode::LabelNode(pos_label label) : Base(label)
	{ }

	LabelNode::LabelNode(pos_label label, vector<LabelNode> subNodes)
		: Base(label, move(subNodes))
	{ }

	/// 这里要保证 subs 的 label 升序排列
	void LabelNode::SetSubs(vector<LabelNode> subNodes)
	{
		_subNodes = move(subNodes);
	}

	vector<LabelNode> LabelNode::GiveSubs()
	{
		return move(_subNodes);
	}

	bool LabelNode::SubsEmpty() const noexcept
	{
		return _subNodes.empty();
	}

	optional<LabelNode> LabelNode::TakeInside(pos_label label)
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
