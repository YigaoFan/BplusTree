#pragma once
#include "LableRelationNode.hpp"

namespace FuncLib::Store
{
	class ObjectRelationTree
	{
	private:
		LableRelationNode _root;
	public:
		static ObjectRelationTree ReadObjRelationTreeFrom(path const& filename)
		{
			// TODO 需要应对第一次建立
			return { };
		}

		static void WriteObjRelationTreeTo(path const& filename, ObjectRelationTree const& tree)
		{
			// TODO
		}
		
		ObjectRelationTree() : _root(0)
		{
		}

		void UpdateWith(PosLableNode auto* topLevelNode)
		{
			_root.UpdateWith(topLevelNode, [](pos_lable lable) { return false; }, [](pos_lable) { });
		}
	};
}
