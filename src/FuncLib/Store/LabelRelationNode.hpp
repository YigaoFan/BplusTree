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
	concept PosLabelNode = requires(T node)
	{
		{ node.Label() } -> IsSameTo<pos_label>;
		{ node.GetLabelSortedSubsEnumerator() } -> Enumerator;
	};

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
		LabelRelationNode(LabelRelationNode const& that) = delete;
		LabelRelationNode& operator= (LabelRelationNode const& that) = delete;
		LabelRelationNode(LabelRelationNode&& that) noexcept = default;
		LabelRelationNode& operator= (LabelRelationNode&& that) noexcept
		{
			this->_label = that._label;
			this->_subNodes = move(that._subNodes);
			return *this;
		}

		~LabelRelationNode() = default;

		void Subs(vector<LabelRelationNode> subNodes);
		vector<LabelRelationNode> GiveSubs()
		{
			return move(_subNodes);
		}

		bool SubsEmpty() const
		{
			return _subNodes.empty();
		}

		pos_label Label() const;
		auto CreateSubNodeEnumerator() { return Collections::CreateEnumerator(_subNodes); }
		auto CreateSubNodeEnumerator() const { return Collections::CreateEnumerator(_subNodes); }
		// 这样哪些需要 release 是不是就不用那个 toDoDelete set 来记了？还需要，有的没 Store 就要 release
		// 那这里就要有某种方法标记它已经 Store 了，不要让那个 set 来 release 了
		/// 这里要求 node 的 Label 和当前 LabelNode 的 Label 一样，否则直接在外层直接操作就好了
		template <typename ReadQuerier, typename Releaser>
		void UpdateWith(PosLabelNode auto* node, ReadQuerier const& read, Releaser const& releaser)
		{
			if (not read(_label))
			{
				return;
			}

			vector<LabelRelationNode *> toDeletes;
			vector<LabelRelationNode> toAdds;
			auto thatSubs = node->GetLabelSortedSubsEnumerator();
			auto thisSubs = this->CreateSubNodeEnumerator();
			while (true)
			{
				LabelRelationNode& oldP = thisSubs.Current();
				auto oldLabel = oldP._label;

				auto& sub = thatSubs.Current();
				auto newLabel = sub.Label();

				if (oldLabel < newLabel)
				{
					toDeletes.push_back(&oldP);
					if (not thisSubs.MoveNext())
					{
						goto ProcessRemainNew;
					}
				}
				else if (oldLabel == newLabel)
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
				else // oldLabel > newLabel 说明 newLabel 没遇到过
				{
					LabelRelationNode n = ConsNodeWith(&sub);
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
					if (x._label < y._label)
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
			releaser(_label);
			for (auto& n : _subNodes)
			{
				n.ReleaseAll(releaser);
			}

			_subNodes.clear();
		}

		/// 由于是 internal 的，所以不检查当前 node 的 label
		optional<LabelRelationNode> TakeInternal(pos_label label)
		{
			if (_subNodes.empty())
			{
				return {};
			}

			for (size_t i = 0; auto& n : _subNodes)
			{
				if (n.Label() == label)
				{
					auto des = move(n);
					Erase(i, _subNodes);
					return des;
				}
				else if (auto r = n.TakeInternal(label); r.has_value())
				{
					return r;
				}
			}

			return { };
		}

		void AddSub(LabelRelationNode node);
	};	
}