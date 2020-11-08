#pragma once
#include <string>
#include <vector>
#include "LableRelationNode.hpp"
#include "LableTree.hpp"
#include "../../Basic/Exception.hpp"
#include "FileReader.hpp"
#include "ObjectBytes.hpp"
#include "../Persistence/FriendFuncLibDeclare.hpp"
#include "FreeNodes.hpp"

namespace FuncLib::Store
{
	using Basic::InvalidOperationException;
	using ::std::to_string;
	using ::std::vector;

	class ObjectRelationTree : private LableTree
	{
	private:
		// vector<LableRelationNode> _nodes;
		FreeNodes _freeNodes;

	public:
		static ObjectRelationTree ReadObjRelationTreeFrom(FileReader* reader);
		static void WriteObjRelationTree(ObjectRelationTree const& tree, ObjectBytes* writer);
		ObjectRelationTree() = default;

		/// 注意下面三个函数全是顶层的
		void Add(PosLableNode auto* topLevelNode)
		{
			this->AddSub(LableRelationNode::ConsNodeWith(topLevelNode));
			// 也要 Complete 来处理 subs
		}

		template <typename Releaser>
		void UpdateWith(PosLableNode auto* topLevelNode, Releaser const& releaser)
		{
			// 上层的调用位置保证这里的 topLevelNode 一定是已经存在于磁盘中的 lable
			// 所以这里 lable 一定在这里的 nodes(tree 和 free node) 之中
			// 所以这里的每次的 update 就是要在 nodes 里找，然后继续向下 update
			// 新增的要在 nodes 里查找，不在再创建新的，
			// 删掉的就放到 FreeNode 里
			// 从旧的里面抽取需要的，组成新的
			// 最后以顶层位置加到旧的里

			// TODO releaser and read
			
			// for (auto& n : _nodes)
			// {
			// 	if (auto l = topLevelNode->Lable(); l == n.Lable())
			// 	{
			// 		n.UpdateWith(topLevelNode, [](pos_lable lable) { return false; }, releaser);
			// 		return;
			// 	}
			// }

			throw InvalidOperationException("Update error: ObjectRelationTree not have lable"+ to_string(topLevelNode->Lable()));
		}

		// 下面这两个函数，递归这种把多余的不同的提出来的方法很棒
		template <typename NodeTaker, typename NodeCollector>
		void Complete(LableRelationNode* newNode, NodeTaker take, NodeCollector collect)
		{
			if (auto r = take(newNode->Lable()); r.has_value())
			{
				auto& oldNode = move(r.value());
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
		void Complete(LableRelationNode* newNode, LableRelationNode oldNode, NodeTaker take, NodeCollector collect)
		{
			auto oldSubs = oldNode.CreateSubNodeEnumerator();
			auto newSubs = newNode->CreateSubNodeEnumerator();
			vector<LableRelationNode *> toCollects;

			while (true)
			{
				auto& oldSub = oldSubs.Current();// 这里的 oldSub 是个引用!!! TODO
				auto oldLable = oldSub.Lable();

				auto& newSub = newSubs.Current();
				auto newLable = newSub.Lable();

				if (oldLable < newLable)
				{
					toCollects.push_back(&oldSub);

					if (not oldSubs.MoveNext())
					{
						goto ProcessRemainNew;
					}
				}
				else if (oldLable == newLable)
				{
					// 把 oldSub 从 oldNode 里面 move 出来!!!
					Complete(&newSub, oldSub, take, collect);

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
				else // oldLable > newLable 说明 newLable 没遇到过
				{
					Complete(newSub, take, collect);

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

		ProcessRemainNew:

			for (auto p : toCollects)
			{
				// move(!!!) these node from oldNode to collect
				// oldNode should delete these
			}
		}

		template <typename Releaser>
		void DeleteTopLevelObj(pos_lable lable, Releaser const& releaser)
		{
			// for (auto i = 0; auto& n : _fileRoot)
			// {
			// 	if (lable == n.Lable())
			// 	{
			// 		n.ReleaseAll(releaser);
			// 		_nodes.erase(_nodes.begin() + i);
			// 		return;
			// 	}

			// 	++i;
			// }

			// throw exception??
		}
	private:
		ObjectRelationTree(LableRelationNode node);
	};
}
