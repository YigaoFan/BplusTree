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

		auto MakeTakeCollectPairWith(set<pos_label>* takeRecords)
		{
			auto take = [=, this](pos_label label)
			{
				// 可能这样操作还是太粗了 TODO
				takeRecords->insert(label);
				return Take(label);
			};
			auto collect = [=, this](LabelNode node)
			{
				// 可能这样操作还是太粗了 TODO
				takeRecords->erase(node.Label());
				return Collect(move(node));
			};

			return pair(take, collect);
		}
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
			set<pos_label> takeRecords;
			auto [take, collect] = MakeTakeCollectPairWith(&takeRecords);
			Complete(&newTopLevelNode, take, collect);
			// 目前要 take 只能在从 ObjectRelationTree 里拿
			if (not takeRecords.empty())
			{

			}
			Base::AddSub(newTopLevelNode.GenLabelNode());
		}

		void Free(PosLabelNode auto* topLevelNode)
		{
			auto newTopLevelNode = ReadStateLabelNode::ConsNodeWith(topLevelNode);
			set<pos_label> takeRecords;
			auto [take, collect] = MakeTakeCollectPairWith(&takeRecords);
			Complete(&newTopLevelNode, take, collect);
			// 目前要 take 只能在从 ObjectRelationTree 里拿
			if (not takeRecords.empty())
			{
			}
			_freeNodes.AddSub(newTopLevelNode.GenLabelNode());
		}

		/// releaser's arg type is pos_label
		void ReleaseFreeNodes(auto const& releaser)
		{
			_freeNodes.ReleaseAll(releaser);
		}

	private:
		using TakeFunc = optional<LabelNode> (*)(pos_label label);
		using CollectFunc = void (*)(LabelNode node);
		/// complete the content from old tree to new
		void Complete(ReadStateLabelNode* newNode, auto take, auto collect)
		{
			if (auto r = take(newNode->Label()); r.has_value())
			{
				auto oldNode = move(r.value());
				Complete(newNode, move(oldNode), take, collect);
			}
			else
			{
				auto e = newNode->CreateSortedSubNodeEnumerator();
				while (e.MoveNext())
				{
					Complete(&e.Current(), take, collect);
				}
			}
		}

		/// Precondition: newNode->Label() == oldNode.Label()
		void Complete(ReadStateLabelNode* newNode, LabelNode oldNode, auto take, auto collect)
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
					Complete(&newSub, move(oldSub), take, collect);

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
					Complete(&newSub, take, collect);

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
				Complete(&newSubs.Current(), take, collect);
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
