#pragma once
#include <vector>
#include "StaticConfig.hpp"
#include "../../Btree/Enumerator.hpp"

namespace FuncLib::Store
{
	using ::std::vector;

	template <typename Derived>
	class LabelNodeBase
	{
	protected:
		pos_label _label;
		vector<Derived> _subNodes;

	public:
		LabelNodeBase(pos_label label) : _label(label)
		{
		}

		LabelNodeBase(pos_label label, vector<Derived> subNodes)
			: _label(label), _subNodes(move(subNodes))
		{
		}

		pos_label Label() const noexcept { return _label; }
		auto CreateSubNodeEnumerator()       { return Collections::CreateRefEnumerator(_subNodes); }
		auto CreateSubNodeEnumerator() const { return Collections::CreateRefEnumerator(_subNodes); }

		void AddSub(Derived node)
		{
			for (auto it = _subNodes.begin(); it != _subNodes.end(); ++it)
			{
				if (node.Label() <= it->Label())
				{
					_subNodes.insert(it, move(node));
					return;
				}
			}

			_subNodes.push_back(move(node));
		}
	};	
}
