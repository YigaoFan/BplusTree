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

		bool SubsEmpty() const { return _subNodes.empty(); }
		pos_label Label() const noexcept { return _label; }
		auto CreateSortedSubNodeEnumerator()       { return Collections::CreateRefEnumerator(_subNodes); }
		auto CreateSortedSubNodeEnumerator() const { return Collections::CreateRefEnumerator(_subNodes); }

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

		Derived* ConstructSub(pos_label label)
		{
			for (auto it = _subNodes.begin(); it != _subNodes.end(); ++it)
			{
				if (label <= it->Label())
				{
					return &(*_subNodes.insert(it, Derived(label)));
				}
			}

			_subNodes.push_back(Derived(label));
			return &_subNodes.back();
		}
	};	
}
