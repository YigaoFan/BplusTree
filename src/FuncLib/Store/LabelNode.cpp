#include "LabelNode.hpp"

namespace FuncLib::Store
{
	using ::std::move;

	vector<LabelNode> LabelNode::GiveSubs()
	{
		return move(_subNodes);
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
