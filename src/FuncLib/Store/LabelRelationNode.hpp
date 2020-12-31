#pragma once
#include <vector>
#include <optional>
#include "StaticConfig.hpp"
#include "../../Btree/Enumerator.hpp"
#include "VectorUtil.hpp"
#include "PosLabelNodeConcept.hpp"

namespace FuncLib::Store
{
	using ::std::optional;
	using ::std::vector;

	class LabelRelationNode
	{
	private:
		pos_label _label;
		vector<LabelRelationNode> _subNodes;

	public:
		static LabelRelationNode ConsNodeWith(PosLabelNode auto* labelNode)
		{
			vector<LabelRelationNode> subNodes;

			auto e = labelNode->GetLabelSortedSubsEnumerator();
			while (e.MoveNext())
			{
				subNodes.push_back(ConsNodeWith(&e.Current()));
			}

			return LabelRelationNode(labelNode->Label(), move(subNodes));
		}

		LabelRelationNode(pos_label label);
		LabelRelationNode(pos_label label, vector<LabelRelationNode> subNodes);

		pos_label Label() const;
		void SetSubs(vector<LabelRelationNode> subNodes);
		vector<LabelRelationNode> GiveSubs();
		bool SubsEmpty() const noexcept;
		void AddSub(LabelRelationNode node);
		/// 由于是 inside 的，所以不检查当前 node 的 label
		optional<LabelRelationNode> TakeInside(pos_label label);
		auto CreateSubNodeEnumerator() { return Collections::CreateRefEnumerator(_subNodes); }
		auto CreateSubNodeEnumerator() const { return Collections::CreateRefEnumerator(_subNodes); }
		// 这样哪些需要 release 是不是就不用那个 toDoDelete set 来记了？还需要，有的没 Store 就要 release
		// 那这里就要有某种方法标记它已经 Store 了，不要让那个 set 来 release 了
	
		template <typename Releaser>
		void ReleaseAll(Releaser const& releaser)
		{
			releaser(_label);
			for (auto& n : _subNodes)
			{
				n.ReleaseAll(releaser);
			}

			_subNodes.clear();
		}
	};	
}