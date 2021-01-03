#pragma once
#include <set>
#include <vector>
#include <utility>
#include <optional>
#include <functional>
#include "LabelNode.hpp"
#include "LabelTree.hpp"
#include "FileReader.hpp"
#include "ObjectBytes.hpp"
#include "../Persistence/FriendFuncLibDeclare.hpp"
#include "FreeNodes.hpp"
#include "ReadStateLabelNode.hpp"

namespace FuncLib::Store
{
	using ::std::function;
	using ::std::move;
	using ::std::optional;
	using ::std::pair;
	using ::std::set;
	using ::std::vector;

	class ObjectRelationTree : private LabelTree
	{
	private:
		using Base = LabelTree;
		FreeNodes _freeNodes;
		set<pos_label> _existLabels;

	public:
		static ObjectRelationTree ReadObjRelationTreeFrom(FileReader* reader);
		static void WriteObjRelationTree(ObjectRelationTree const& tree, ObjectBytes* writer);
		static set<pos_label> GetLabelsFrom(LabelNode const& node)
		{
			set<pos_label> labels;

			auto e = node.CreateSortedSubNodeEnumerator();
			while (e.MoveNext())
			{
				labels.insert(e.Current().Label());
			}

			return labels;
		}

		ObjectRelationTree(LabelNode root, set<pos_label> existLabels);

		void UpdateWith(PosLabelNode auto* topLevelNode)
		{
			// 这里构造的新节点的叶子节点就是读取的终点，没有读到的话，就代表被删掉了
			auto newTopLevelNode = ReadStateLabelNode::ConsNodeWith(topLevelNode);
			Complete(&newTopLevelNode);
			Base::AddSub(newTopLevelNode.GenLabelNode());
		}

		void Free(PosLabelNode auto* topLevelNode)
		{
			auto newTopLevelNode = ReadStateLabelNode::ConsNodeWith(topLevelNode);
			Complete(&newTopLevelNode);
			_freeNodes.AddSub(newTopLevelNode.GenLabelNode());
		}

		/// releaser's arg type is pos_label
		void ReleaseFreeNodes(auto const& releaser)
		{
			_freeNodes.ReleaseAll(releaser);
		}

	private:
		/// complete the content from old tree to new
		void Complete(ReadStateLabelNode* newNode)
		{
			if (auto r = Take(newNode->Label()); r.has_value())
			{
				auto oldNode = move(r.value());
				Complete(newNode, move(oldNode));
			}
			else
			{
				auto e = newNode->CreateSortedSubNodeEnumerator();
				while (e.MoveNext())
				{
					Complete(&e.Current());
				}
			}
		}

		/// Precondition: newNode->Label() == oldNode.Label()
		void Complete(ReadStateLabelNode* newNode, LabelNode oldNode)
		{
			if (oldNode.SubsEmpty()) { return; }
			if (newNode->SubsEmpty())
			{
				if (newNode->Read)
				{
					for (auto& n : oldNode.GiveSubs())
					{
						Collect(move(n));
					}
				}
				else
				{
					newNode->SetSubs(oldNode.GiveSubs());
				}
				return;
			}

			auto oldSubs = oldNode.CreateSortedSubNodeEnumerator();
			auto newSubs = newNode->CreateSortedSubNodeEnumerator();
			oldSubs.MoveNext();
			newSubs.MoveNext();

			while (true)
			{
				auto& oldSub = oldSubs.Current();
				auto oldLabel = oldSub.Label();

				auto& newSub = newSubs.Current();
				auto newLabel = newSub.Label();

				if (oldLabel < newLabel)
				{
					Collect(oldSub);

					if (not oldSubs.MoveNext()) { goto ProcessRemainNew; }
				}
				else if (oldLabel == newLabel)
				{
					Complete(&newSub, move(oldSub));

					auto hasOld = oldSubs.MoveNext();
					auto hasNew = newSubs.MoveNext();

					if (hasOld)
					{
						if (hasNew) { continue; }
						else { goto ProcessRemainOld; }
					}
					else
					{
						if (hasNew) { goto ProcessRemainNew; }
						else { break; }
					}
				}
				else // oldLabel > newLabel 说明 newLabel 没遇到过，但可能来自 ObjectRelationTree 的其他地方
				{
					Complete(&newSub);

					if (not newSubs.MoveNext()) { goto ProcessRemainOld; }
				}
			}

		ProcessRemainOld:
			do
			{
				Collect(oldSubs.Current());
			} while (oldSubs.MoveNext());

		ProcessRemainNew:
			do
			{
				Complete(&newSubs.Current());
			} while (newSubs.MoveNext());
		}

		optional<LabelNode> Take(pos_label label)
		{
			if (not _existLabels.contains(label))
			{
				return {};
			}
			
			if (auto r = Base::Take(label); r.has_value())
			{
				return r;
			}

			return _freeNodes.Take(label);
			// 如果这个 Node 是已有的（因为 ObjectRelationTree 就是全局，所以可以直接搜索），就要在这里等
		}

		void Collect(LabelNode node)
		{
			auto labels = GetLabelsFrom(node);
			_existLabels.insert(labels.begin(), labels.end());
			// 借还契约 TODO
			_freeNodes.AddSub(move(node));
		}
	};
}
