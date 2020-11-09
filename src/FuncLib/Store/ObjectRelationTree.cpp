#include "ObjectRelationTree.hpp"
#include "ObjectBytes.hpp"
#include "../Persistence/ByteConverter.hpp"
#include "Generator.hpp"
#include <optional>

namespace FuncLib::Store
{
	using ::std::optional;

	constexpr pos_lable NonLable = 0;// 树中不能出现 0

	Generator<optional<vector<pos_lable>>> ReadLables(FileReader* reader)
	{

		for (vector<pos_lable> lables;;)
		{
			auto lable = ByteConverter<pos_lable>::ReadOut(reader);
			if (NonLable == lable)
			{
				if (lables.empty())
				{
					co_yield {};
				}
				else
				{
					co_yield move(lables);
				}
			}
			else
			{
				lables.push_back(lable);
			}			
		}
	}

	vector<LableRelationNode> ConsNodes(Generator<optional<vector<pos_lable>>>* generator)
	{
		generator->Resume();

		if (not generator->Current().has_value())
		{
			return {};
		}

		vector<LableRelationNode> nodes;
		// will be changed in below inner ConsNodes call, so need to save it
		auto lables = generator->Current().value();
		for (auto l : lables)
		{
			auto n = LableRelationNode(l);
			auto subNodes = ConsNodes(generator);
			n.Subs(move(subNodes));

			nodes.push_back(move(n));
		}

		return nodes;
	}

	ObjectRelationTree ObjectRelationTree::ReadObjRelationTreeFrom(FileReader* reader)
	{
		auto fileLable = ByteConverter<pos_lable>::ReadOut(reader);
		auto gen = ReadLables(reader);

		return { LableRelationNode(fileLable, ConsNodes(&gen)) };
	}

	void WriteSubLableOf(LableRelationNode const* node, ObjectBytes* writer)
	{
		auto e1 = node->CreateSubNodeEnumerator();
		while (e1.MoveNext())
		{
			auto l = e1.Current().Lable();
			ByteConverter<pos_lable>::WriteDown(l, writer);
		}

		ByteConverter<pos_lable>::WriteDown(NonLable, writer);

		auto e2 = node->CreateSubNodeEnumerator();
		while (e2.MoveNext())
		{
			WriteSubLableOf(&e2.Current(), writer);
		}
	}

	// 上下这两处代码重复了
	void ObjectRelationTree::WriteObjRelationTree(ObjectRelationTree const& tree, ObjectBytes* writer)
	{
		ByteConverter<pos_lable>::WriteDown(tree._fileRoot.Lable(), writer);
		WriteSubLableOf(&tree._fileRoot, writer);
	}
}
