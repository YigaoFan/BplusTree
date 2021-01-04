#include <vector>
#include "ObjectRelationTree.hpp"
#include "../Persistence/ByteConverter.hpp"
#include "../../Btree/Generator.hpp"

namespace FuncLib::Store
{
	using Collections::Generator;
	using ::std::move;
	using ::std::vector;

	// 但 FileLabel 是 0... TODO
	constexpr pos_label NonLabel = 0;// 树中不能出现 0

	Generator<optional<vector<pos_label>>> ReadLabels(FileReader* reader)
	{

		for (vector<pos_label> labels;;)
		{
			auto label = ByteConverter<pos_label>::ReadOut(reader);
			if (NonLabel == label)
			{
				if (labels.empty())
				{
					co_yield {};
				}
				else
				{
					co_yield move(labels);
				}
			}
			else
			{
				labels.push_back(label);
			}			
		}
	}

	vector<LabelNode> ConsNodes(Generator<optional<vector<pos_label>>>* generator)
	{
		generator->MoveNext();

		if (not generator->Current().has_value())
		{
			return {};
		}

		vector<LabelNode> nodes;
		// will be changed in below inner ConsNodes call, so need to save it
		auto labels = generator->Current().value();
		for (auto l : labels)
		{
			auto subNodes = ConsNodes(generator);
			auto n = LabelNode(l, move(subNodes));

			nodes.push_back(move(n));
		}

		return nodes;
	}

	ObjectRelationTree ObjectRelationTree::ReadObjRelationTreeFrom(FileReader* reader)
	{
		auto fileLabel = ByteConverter<pos_label>::ReadOut(reader);
		auto gen = ReadLabels(reader);
		auto root = LabelNode(fileLabel, ConsNodes(&gen));
		return { move(root) };
	}

	void WriteSubLabelOf(LabelNode const* node, ObjectBytes* writer)
	{
		auto e1 = node->CreateSortedSubNodeEnumerator();
		while (e1.MoveNext())
		{
			auto l = e1.Current().Label();
			ByteConverter<pos_label>::WriteDown(l, writer);
		}

		ByteConverter<pos_label>::WriteDown(NonLabel, writer);

		auto e2 = node->CreateSortedSubNodeEnumerator();
		while (e2.MoveNext())
		{
			WriteSubLabelOf(&e2.Current(), writer);
		}
	}

	ObjectRelationTree::ObjectRelationTree(LabelTree tree, FreeNodes freeNodes)
		: Base(move(tree)), _freeNodes(move(freeNodes))
	{ }

	void ObjectRelationTree::WriteObjRelationTree(ObjectRelationTree const& tree, ObjectBytes* writer)
	{
		ByteConverter<pos_label>::WriteDown(tree._fileRoot.Label(), writer);
		WriteSubLabelOf(&tree._fileRoot, writer);
	}

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
		if (oldNode.SubsEmpty())
		{
			return;
		}
		if (newNode->SubsEmpty())
		{
			if (newNode->Read)
			{
				for (auto &n : oldNode.GiveSubs())
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
			auto &oldSub = oldSubs.Current();
			auto oldLabel = oldSub.Label();

			auto &newSub = newSubs.Current();
			auto newLabel = newSub.Label();

			if (oldLabel < newLabel)
			{
				Collect(oldSub);

				if (not oldSubs.MoveNext())
				{
					goto ProcessRemainNew;
				}
			}
			else if (oldLabel == newLabel)
			{
				Complete(&newSub, move(oldSub));

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
			else // oldLabel > newLabel 说明 newLabel 没遇到过，但可能来自 ObjectRelationTree 的其他地方
			{
				Complete(&newSub);

				if (not newSubs.MoveNext())
				{
					goto ProcessRemainOld;
				}
			}
		}

	ProcessRemainOld:
		do
		{
			Collect(oldSubs.Current());
		} while (oldSubs.MoveNext());
		return;

	ProcessRemainNew:
		do
		{
			Complete(&newSubs.Current());
		} while (newSubs.MoveNext());
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
