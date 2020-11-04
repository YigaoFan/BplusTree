#pragma once
#include <vector>
#include "StaticConfig.hpp"
#include "../../Btree/Enumerator.hpp"

#include "../../Basic/Concepts.hpp" // temp

namespace FuncLib::Store
{
	using ::std::move;
	using ::std::vector;

	using Basic::IsConvertibleTo;
	using Basic::IsSameTo;
	using Collections::CreateEnumerator;

	template <typename T>
	concept Enumerator = requires(T e)
	{
		e.MoveNext();
		e.Current();
	};

	template <typename T>
	concept PosLableNode = requires(T node)
	{
		{ node.Lable() } -> IsSameTo<pos_lable>;
		{ node.GetLableSortedSubsEnumerator() } -> Enumerator;
	};

	class LableRelationNode
	{
	private:
		pos_lable _lable;
		vector<LableRelationNode*> _subLables;// 用 shared_ptr TODO
	public:
		static LableRelationNode* ConsNodePtrWith(PosLableNode auto* lableNode)
		{
			vector<LableRelationNode*> subNodes;

			auto e = lableNode->GetLableSortedSubsEnumerator();
			while (e.MoveNext())
			{
				subNodes.push_back(ConsNodePtrWith(e.Current()));
			}

			return new LableRelationNode(lableNode->Lable(), move(subNodes));
		}

		static LableRelationNode ConsNodeWith(PosLableNode auto* lableNode)
		{
			vector<LableRelationNode*> subNodes;

			auto e = lableNode->GetLableSortedSubsEnumerator();
			while (e.MoveNext())
			{
				subNodes.push_back(ConsNodePtrWith(e.Current()));
			}

			return LableRelationNode(lableNode->Lable(), move(subNodes));
		}

		LableRelationNode(pos_lable lable);
		LableRelationNode(LableRelationNode const& that) = delete;
		LableRelationNode& operator= (LableRelationNode const& that) = delete;
		LableRelationNode(LableRelationNode&& that) noexcept = default;
		LableRelationNode& operator= (LableRelationNode&& that) noexcept
		{
			this->_lable = that._lable;
			this->_subLables = move(that._subLables);
			return *this;
		}

		~LableRelationNode();

		pos_lable Lable() const;

		// 这样哪些需要 release 是不是就不用那个 toDoDelete set 来记了？还需要，有的没 Store 就要 release
		// 那这里就要有某种方法标记它已经 Store 了，不要让那个 set 来 release 了
		/// 这里要求 node 的 Lable 和当前 LableNode 的 Lable 一样，否则直接在外层直接操作就好了
		template <typename ReadQuerier, typename Releaser>
		void UpdateWith(PosLableNode auto* node, ReadQuerier const& read, Releaser const& releaser)
		{
			if (not read(_lable))
			{
				return;
			}

			vector<LableRelationNode *> toDeletes;
			vector<LableRelationNode *> toAdds;
			auto thatSubs = node->GetLableSortedSubsEnumerator();
			auto thisSubs = CreateEnumerator(_subLables);
			while (true)
			{
				LableRelationNode *oldP = thisSubs.Current();
				auto oldLable = oldP->_lable;

				auto subPtr = thatSubs.Current();
				auto newLable = subPtr->Lable();

				if (oldLable < newLable)
				{
					toDeletes.push_back(oldP);
					if (not thisSubs.MoveNext())
					{
						goto ProcessRemainNew;
					}
				}
				else if (oldLable == newLable)
				{
					oldP->UpdateWith(subPtr, read, releaser);
					auto hasOld = thisSubs.MoveNext();
					auto hasNew = thatSubs.MoveNext();

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
					LableRelationNode *n = ConsNodePtrWith(subPtr);
					toAdds.push_back(n);

					if (not thatSubs.MoveNext())
					{
						goto ProcessRemainOld;
					}
				}

			ProcessRemainOld:
				do
				{
					toDeletes.push_back(thisSubs.Current());
				} while (thisSubs.MoveNext());
				break;

			ProcessRemainNew:
				do
				{
					toAdds.push_back(ConsNodePtrWith(thatSubs.Current()));
				} while (thatSubs.MoveNext());
				break;
			}

			for (auto x : toDeletes)
			{
				x->ReleaseAll(releaser);
				delete x;
				for (size_t i = 0; i < _subLables.size(); ++i)
				{
					if (x == _subLables[i])
					{
						_subLables.erase(_subLables.begin() + i);
						break;
					}
				}
			}

			for (auto x : toAdds)
			{
				for (size_t i = 0; auto y : _subLables)
				{
					if (x->_lable < y->_lable)
					{
						_subLables.insert(_subLables.begin() + i, x);
						break;
					}

					++i;
				}
			}
		}
	
		template <typename Releaser>
		void ReleaseAll(Releaser const& releaser)
		{
			releaser(_lable);
			for (auto p : _subLables)
			{
				p->ReleaseAll(releaser);
				delete p;
			}

			_subLables.clear();
		}
		
	private:
		LableRelationNode(pos_lable lable, vector<LableRelationNode*> subNodes);

	};	
}
