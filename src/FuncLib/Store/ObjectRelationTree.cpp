#include "ObjectRelationTree.hpp"
#include "ObjectBytes.hpp"
#include "../Persistence/ByteConverter.hpp"
#include "Generator.hpp"
#include <optional>

namespace FuncLib::Store
{
	using ::std::optional;

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

	vector<LabelRelationNode> ConsNodes(Generator<optional<vector<pos_label>>>* generator)
	{
		generator->MoveNext();

		if (not generator->Current().has_value())
		{
			return {};
		}

		vector<LabelRelationNode> nodes;
		// will be changed in below inner ConsNodes call, so need to save it
		auto labels = generator->Current().value();
		for (auto l : labels)
		{
			auto n = LabelRelationNode(l);
			auto subNodes = ConsNodes(generator);
			n.Subs(move(subNodes));

			nodes.push_back(move(n));
		}

		return nodes;
	}

	ObjectRelationTree ObjectRelationTree::ReadObjRelationTreeFrom(FileReader* reader)
	{
		auto fileLabel = ByteConverter<pos_label>::ReadOut(reader);
		auto gen = ReadLabels(reader);

		return { LabelRelationNode(fileLabel, ConsNodes(&gen)) };
	}

	void WriteSubLabelOf(LabelRelationNode const* node, ObjectBytes* writer)
	{
		auto e1 = node->CreateSubNodeEnumerator();
		while (e1.MoveNext())
		{
			auto l = e1.Current().Label();
			ByteConverter<pos_label>::WriteDown(l, writer);
		}

		ByteConverter<pos_label>::WriteDown(NonLabel, writer);

		auto e2 = node->CreateSubNodeEnumerator();
		while (e2.MoveNext())
		{
			WriteSubLabelOf(&e2.Current(), writer);
		}
	}

	void ObjectRelationTree::WriteObjRelationTree(ObjectRelationTree const& tree, ObjectBytes* writer)
	{
		ByteConverter<pos_label>::WriteDown(tree._fileRoot.Label(), writer);
		WriteSubLabelOf(&tree._fileRoot, writer);
	}
}
