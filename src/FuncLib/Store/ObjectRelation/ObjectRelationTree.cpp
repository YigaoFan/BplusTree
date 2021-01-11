#include <vector>
#include <exception>
#include "ObjectRelationTree.hpp"

namespace FuncLib::Store::ObjectRelation
{
	using ::std::move;
	using ::std::out_of_range;
	using ::std::vector;

	ObjectRelationTree::ObjectRelationTree(LabelTree tree, FreeNodes freeNodes)
		: Base(move(tree)), _freeNodes(move(freeNodes))
	{ }

	void ObjectRelationTree::UpdateWith(ReadStateLabelNode topNode)
	{
		Complete(&topNode);
		Base::AddSub(topNode.GenLabelNode());
	}

	void ObjectRelationTree::Free(ReadStateLabelNode topNode)
	{
		Complete(&topNode);
		_freeNodes.AddSub(topNode.GenLabelNode());
	}

	void ObjectRelationTree::Complete(ReadStateLabelNode *newNode)
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

	void ObjectRelationTree::Complete(ReadStateLabelNode *newNode, LabelNode oldNode)
	{
		if (newNode->SubsEmpty())
		{
			if (not newNode->Read)
			{
				newNode->SetSubs(oldNode.GiveSubs());
				return;
			}
		}

		auto oldSubs = oldNode.CreateSortedSubNodeEnumerator();
		auto newSubs = newNode->CreateSortedSubNodeEnumerator();
		unsigned char state = 0;
		constexpr unsigned char newCurrentValidFlag = 0b0000'0001;
		constexpr unsigned char oldCurrentValidFlag = 0b0000'0010;

		auto setOldCurrent = [&state](bool moved)
		{
			if (moved)
			{
				state |= oldCurrentValidFlag;
			}
			else
			{
				state &= ~oldCurrentValidFlag;
			}
		};
		auto setNewCurrent = [&state](bool moved)
		{
			if (moved)
			{
				state |= newCurrentValidFlag;
			}
			else
			{
				state &= ~newCurrentValidFlag;
			}
		};
		setOldCurrent(oldSubs.MoveNext());
		setNewCurrent(newSubs.MoveNext());

		while (true)
		{
			switch (state)
			{
			case 0:
				return;

			case 1:
				do
				{
					Complete(&newSubs.Current());
				} while (newSubs.MoveNext());
				return;

			case 2:
				do
				{
					Collect(oldSubs.Current());
				} while (oldSubs.MoveNext());
				return;

			case 3:
				{
					auto &oldSub = oldSubs.Current();
					auto oldLabel = oldSub.Label();

					auto &newSub = newSubs.Current();
					auto newLabel = newSub.Label();

					if (oldLabel < newLabel)
					{
						Collect(oldSub);
						setOldCurrent(oldSubs.MoveNext());
					}
					else if (oldLabel == newLabel)
					{
						Complete(&newSub, move(oldSub));
						setOldCurrent(oldSubs.MoveNext());
						setNewCurrent(newSubs.MoveNext());
					}
					else // oldLabel > newLabel 说明 newLabel 没遇到过，可能来自 ObjectRelationTree 的其他地方或者是新加的
					{
						Complete(&newSub);
						setNewCurrent(newSubs.MoveNext());
					}
				}
				break;

			default:
				throw out_of_range("state out of range");
			}
		}
	}

	optional<LabelNode> ObjectRelationTree::Take(pos_label label)
	{
		if (auto r = Base::Take(label); r.has_value())
		{
			return r;
		}

		return _freeNodes.Take(label);
	}

	void ObjectRelationTree::Collect(LabelNode node)
	{
		_freeNodes.AddSub(move(node));
	}
}
