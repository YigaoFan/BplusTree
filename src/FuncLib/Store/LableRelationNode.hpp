#pragma once
#include <vector>
#include <optional>
#include "StaticConfig.hpp"
#include "../../Btree/Enumerator.hpp"
#include "VectorUtil.hpp"

#include "../../Basic/Concepts.hpp" // temp

namespace FuncLib::Store
{
	using ::std::move;
	using ::std::optional;
	using ::std::vector;

	using Basic::IsConvertibleTo;
	using Basic::IsSameTo;

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
		vector<LableRelationNode> _subNodes;
	public:
		static LableRelationNode ConsNodeWith(PosLableNode auto* lableNode)
		{
			vector<LableRelationNode> subNodes;

			auto e = lableNode->GetLableSortedSubsEnumerator();
			while (e.MoveNext())
			{
				subNodes.push_back(ConsNodeWith(&e.Current()));
			}

			return LableRelationNode(lableNode->Lable(), move(subNodes));
		}

		LableRelationNode(pos_lable lable);
		LableRelationNode(pos_lable lable, vector<LableRelationNode> subNodes);
		LableRelationNode(LableRelationNode const& that) = delete;
		LableRelationNode& operator= (LableRelationNode const& that) = delete;
		LableRelationNode(LableRelationNode&& that) noexcept = default;
		LableRelationNode& operator= (LableRelationNode&& that) noexcept
		{
			this->_lable = that._lable;
			this->_subNodes = move(that._subNodes);
			return *this;
		}

		~LableRelationNode() = default;

		void Subs(vector<LableRelationNode> subNodes);
		pos_lable Lable() const;
		auto CreateSubNodeEnumerator() { return Collections::CreateEnumerator(_subNodes); }
		auto CreateSubNodeEnumerator() const { return Collections::CreateEnumerator(_subNodes); }
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
			vector<LableRelationNode> toAdds;
			auto thatSubs = node->GetLableSortedSubsEnumerator();
			auto thisSubs = this->CreateSubNodeEnumerator();
			while (true)
			{
				LableRelationNode& oldP = thisSubs.Current();
				auto oldLable = oldP._lable;

				auto& sub = thatSubs.Current();
				auto newLable = sub.Lable();

				if (oldLable < newLable)
				{
					toDeletes.push_back(&oldP);
					if (not thisSubs.MoveNext())
					{
						goto ProcessRemainNew;
					}
				}
				else if (oldLable == newLable)
				{
					oldP.UpdateWith(&sub, read, releaser);
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
					LableRelationNode n = ConsNodeWith(&sub);
					toAdds.push_back(move(n));

					if (not thatSubs.MoveNext())
					{
						goto ProcessRemainOld;
					}
				}

			ProcessRemainOld:
				do
				{
					toDeletes.push_back(&thisSubs.Current());
				} while (thisSubs.MoveNext());
				break;

			ProcessRemainNew:
				do
				{
					toAdds.push_back(ConsNodeWith(&thatSubs.Current()));
				} while (thatSubs.MoveNext());
				break;
			}

			for (auto x : toDeletes)
			{
				x->ReleaseAll(releaser);
				for (size_t i = 0; i < _subNodes.size(); ++i)
				{
					if (x == (&_subNodes[i]))
					{
						_subNodes.erase(_subNodes.begin() + i);
						break;
					}
				}
			}

			for (auto& x : toAdds)
			{
				for (size_t i = 0; auto& y : _subNodes)
				{
					if (x._lable < y._lable)
					{
						_subNodes.insert(_subNodes.begin() + i, move(x));
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
			for (auto& n : _subNodes)
			{
				n.ReleaseAll(releaser);
			}

			_subNodes.clear();
		}

		/// 由于是 internal 的，所以不检查当前 node 的 lable
		optional<LableRelationNode> TakeInternal(pos_lable lable)
		{
			if (_subNodes.empty())
			{
				return {};
			}

			for (size_t i = 0; auto& n : _subNodes)
			{
				if (n.Lable() == lable)
				{
					auto des = move(n);
					Erase(i, _subNodes);
					return des;
				}
				else if (auto r = n.TakeInternal(lable); r.has_value())
				{
					return r;
				}
			}

			return { };
		}

		void AddSub(LableRelationNode node);
	};	
}