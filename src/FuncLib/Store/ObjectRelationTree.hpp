#pragma once
#include <vector>
#include <utility>
#include "LabelNode.hpp"
#include "LabelTree.hpp"
#include "FileReader.hpp"
#include "ObjectBytes.hpp"
#include "../Persistence/FriendFuncLibDeclare.hpp"
#include "FreeNodes.hpp"

namespace FuncLib::Store
{
	using ::std::move;
	using ::std::pair;
	using ::std::vector;

	class ObjectRelationTree : private LabelTree
	{
	private:
		FreeNodes _freeNodes;

		auto MakeTakerAndCollector()
		{
			auto take = [&](pos_label label)
			{
				if (auto r = this->Take(label); r.has_value())
				{
					return r;
				}

				return _freeNodes.Take(label);				
			};

			auto collect = [&](LabelNode node)
			{
				_freeNodes.AddSub(move(node));
			};

			return pair(move(take), move(collect));
		}
	public:
		static ObjectRelationTree ReadObjRelationTreeFrom(FileReader* reader);
		static void WriteObjRelationTree(ObjectRelationTree const& tree, ObjectBytes* writer);

		using LabelTree::LabelTree;
		ObjectRelationTree() = default;

		void UpdateWith(PosLabelNode auto* topLevelNode)
		{
			// 上层的调用位置保证这里的 topLevelNode 一定是已经存在于磁盘中的 label
			// 所以这里 label 一定在这里的 nodes(tree 和 free node) 之中
			// 所以这里的每次的 update 就是要在 nodes 里找，然后继续向下 update
			// 新增的要在 nodes 里查找，不在再创建新的，
			// 删掉的就放到 FreeNode 里
			// 从旧的里面抽取需要的，组成新的
			// 最后以顶层位置加到旧的里

			auto [take, collect] = MakeTakerAndCollector();
			auto newTopLevelNode = LabelNode::ConsNodeWith(topLevelNode);
			Complete(&newTopLevelNode, take, collect);
			this->AddSub(move(newTopLevelNode));
		}

		// 下面这两个函数，递归这种把多余的不同的提出来的方法很棒
		template <typename NodeTaker, typename NodeCollector>
		void Complete(LabelNode* newNode, NodeTaker take, NodeCollector collect)
		{
			if (auto r = take(newNode->Label()); r.has_value())
			{
				auto oldNode = move(r.value());
				Complete(newNode, move(oldNode), take, collect);
			}
			else
			{
				auto e = newNode->CreateSubNodeEnumerator();
				while (e.MoveNext())
				{
					Complete(&e.Current(), take, collect);
				}
			}
		}

		template <typename NodeTaker, typename NodeCollector>
		void Complete(LabelNode* newNode, LabelNode oldNode, NodeTaker take, NodeCollector collect)
		{
			// 说明指向的内容没读或者下面没有内容了
			if (newNode->SubsEmpty())
			{
				newNode->SetSubs(oldNode.GiveSubs());
				return;
			}

			auto oldSubsVec = oldNode.GiveSubs();
			auto oldSubs = CreateRefEnumerator(oldSubsVec);
			auto newSubs = newNode->CreateSubNodeEnumerator();
			vector<LabelNode *> toCollects;

			while (true)
			{
				auto& oldSub = oldSubs.Current();
				auto oldLabel = oldSub.Label();

				auto& newSub = newSubs.Current();
				auto newLabel = newSub.Label();

				if (oldLabel < newLabel)
				{
					toCollects.push_back(&oldSub);

					if (not oldSubs.MoveNext())
					{
						goto ProcessRemainNew;
					}
				}
				else if (oldLabel == newLabel)
				{
					// oldSub 被 move 后里面的 subs 就为空了，然后只有 toCollects 后续会 collect 掉
					// 其他的说明已经在 newNode 里存在，不用处理，让它析构就行
					Complete(&newSub, move(oldSub), take, collect);

					auto hasOld = oldSubs.MoveNext();
					auto hasNew = newSubs.MoveNext();

					if (hasOld)
					{
						if (hasNew)
						{
							continue;
						}
						else
						{
							goto ProcessRemainOld;
						}
					}
					else
					{
						if (hasNew)
						{
							goto ProcessRemainNew;
						}
						else
						{
							break;
						}
					}
				}
				else // oldLabel > newLabel 说明 newLabel 没遇到过
				{
					Complete(&newSub, take, collect);

					if (not newSubs.MoveNext())
					{
						goto ProcessRemainOld;
					}
				}
			}

		ProcessRemainOld:
			do
			{
				toCollects.push_back(&oldSubs.Current());
			} while (oldSubs.MoveNext());
			goto CollectNotUsed;

		ProcessRemainNew:
			do
			{
				Complete(&newSubs.Current(), take, collect);
			} while (newSubs.MoveNext());
			goto CollectNotUsed;
			
		CollectNotUsed:
			for (auto ptr : toCollects)
			{
				for (auto& n : oldSubsVec)
				{
					if (ptr == &n)
					{
						collect(move(n));
					}
				}
			}
		}

		void Free(PosLabelNode auto* topLevelNode)
		{
			auto [take, collect] = MakeTakerAndCollector();
			auto newTopLevelNode = LabelNode::ConsNodeWith(topLevelNode);
			Complete(&newTopLevelNode, take, collect);
			_freeNodes.AddSub(move(newTopLevelNode));
		}

		template <typename Releaser>
		void ReleaseAllFreeNode(Releaser const& releaser)
		{
			_freeNodes.ReleaseAll(releaser);
		}
	};
}
