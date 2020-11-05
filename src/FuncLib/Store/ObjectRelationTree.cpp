#include "ObjectRelationTree.hpp"
#include "ObjectBytes.hpp"
#include "../ByteConverter.hpp"

namespace FuncLib::Store
{
	ObjectRelationTree ObjectRelationTree::ReadObjRelationTreeFrom(FileReader* reader)
	{
		// TODO 需要应对第一次建立
		return {};
	}

	void ObjectRelationTree::WriteObjRelationTreeTo(ObjectRelationTree const& tree, ObjectBytes* writer)
	{
		// TODO
	}
}
