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

	bool LabelNode::EqualTo(LabelNode const& that) const
	{
		if (_label == that._label)
		{
			if (_subNodes.size() == that._subNodes.size())
			{
				for (size_t i = 0; i < _subNodes.size(); ++i)
				{
					if (not _subNodes[i].EqualTo(that._subNodes[i]))
					{
						return false;
					}
				}
				return true;
			}
		}

		return false;
	}
}
