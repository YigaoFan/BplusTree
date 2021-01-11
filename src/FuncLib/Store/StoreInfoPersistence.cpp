#include "StoreInfoPersistence.hpp"

namespace FuncLib::Store
{
	vector<LabelNode> ReadOutSubNodes(Generator<optional<vector<pos_label>>>* generator)
	{
		generator->MoveNext();

		if (not generator->Current().has_value())
		{
			return {};
		}

		vector<LabelNode> nodes;
		// will be changed in below inner ReadOutSubNodes call, so need to save it
		auto labels = generator->Current().value();
		for (auto l : labels)
		{
			auto subNodes = ReadOutSubNodes(generator);
			auto n = LabelNode(l, move(subNodes));

			nodes.push_back(move(n));
		}

		return nodes;
	}

	void WriteSubLabelsOf(LabelNode const *node, ObjectBytes *writer)
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
			WriteSubLabelsOf(&e2.Current(), writer);
		}
	}

	void WriteObjRelationTree(ObjectRelationTree const& tree, ObjectBytes* writer)
	{
		ByteConverter<pos_label>::WriteDown(tree._fileRoot.Label(), writer);
		WriteSubLabelsOf(&tree._fileRoot, writer);
	}

	void WriteAllocatedInfoTo(StorageAllocator const &allocator, ObjectBytes *bytes)
	{
		ByteConverter<StorageAllocator>::WriteDown(allocator, bytes);
	}
}