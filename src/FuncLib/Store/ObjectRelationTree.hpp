#pragma once
#include "LableRelationNode.hpp"
#include "FileReader.hpp"
#include "ObjectBytes.hpp"

namespace FuncLib::Store
{
	class ObjectRelationTree
	{
	private:
		vector<LableRelationNode> _nodes;
	public:
		static ObjectRelationTree ReadObjRelationTreeFrom(FileReader* reader);
		static void WriteObjRelationTreeTo(ObjectRelationTree const& tree, ObjectBytes* writer);
		ObjectRelationTree() = default;
		
		void Add(PosLableNode auto* topLevelNode)
		{
			_nodes.push_back(LableRelationNode::ConsNodeWith(topLevelNode));
		}

		template <typename Releaser>
		void UpdateWith(PosLableNode auto* topLevelNode, Releaser const& releaser)
		{
			// 这里取名是不是也要 TopLevel？
			// 那如何删除一个顶层对象呢？
			for (auto& n : _nodes)
			{
				if (auto l = topLevelNode->Lable(); l == n.Lable())
				{
					n.UpdateWith(topLevelNode, [](pos_lable lable) { return false; }, releaser);
					return;
				}
			}

			// throw exception?
		}

		template <typename Releaser>
		void DeleteTopLevelObj(pos_lable lable, Releaser const& releaser)
		{
			for (auto i = 0; auto& n : _nodes)
			{
				if (lable == n.Lable())
				{
					n.ReleaseAll(releaser);
					_nodes.erase(_nodes.begin() + i);
					return;
				}

				++i;
			}

			// throw exception??
		}
	};
}
