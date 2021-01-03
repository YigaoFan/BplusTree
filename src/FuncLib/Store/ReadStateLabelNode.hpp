#pragma once
#include "LabelNodeBase.hpp"
#include "LabelNode.hpp"
#include "PosLabelNodeConcept.hpp"

namespace FuncLib::Store
{
	using ::std::move;

	class ReadStateLabelNode : private LabelNodeBase<ReadStateLabelNode>
	{
	private:
		using Base = LabelNodeBase<ReadStateLabelNode>;

	public:
		static ReadStateLabelNode ConsNodeWith(PosLabelNode auto* labelNode)
		{
			vector<ReadStateLabelNode> subNodes;

			auto e = labelNode->CreateSortedSubNodeEnumerator();
			while (e.MoveNext())
			{
				subNodes.push_back(ConsNodeWith(&e.Current()));
			}

			return ReadStateLabelNode(labelNode->Label(), move(subNodes), labelNode->Written());
		}

		bool const Read = false;

		void SetSubs(vector<LabelNode> subNodes)
		{
			_subNodes = ConstructFrom(subNodes);
		}

		using Base::CreateSortedSubNodeEnumerator;
		using Base::Label;
		using Base::SubsEmpty;

		LabelNode GenLabelNode() const
		{
			vector<LabelNode> subs;

			for (auto& x : _subNodes)
			{
				subs.push_back(x.GenLabelNode());
			}

			return LabelNode(_label, move(subs));
		}

	private:
		ReadStateLabelNode(pos_label label, vector<ReadStateLabelNode> subs, bool read = true)
			: Base(label, move(subs)), Read(read)
		{ }

		static ReadStateLabelNode ConstructFrom(LabelNode const& node)
		{
			vector<ReadStateLabelNode> converteds;
			auto e = node.CreateSortedSubNodeEnumerator();
			while (e.MoveNext())
			{
				converteds.push_back(ConstructFrom(e.Current()));
			}

			return ReadStateLabelNode(node.Label(), move(converteds));
		}

		static vector<ReadStateLabelNode> ConstructFrom(vector<LabelNode> const& nodes)
		{
			vector<ReadStateLabelNode> converteds;

			for (auto& x : nodes)
			{
				converteds.push_back(ConstructFrom(x));
			}

			return converteds;
		}
	};
}
