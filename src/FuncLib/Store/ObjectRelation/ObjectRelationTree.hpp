#pragma once
#include <utility>
#include <optional>
#include "LabelNode.hpp"
#include "LabelTree.hpp"
#include "../../Persistence/IWriterIReaderConcept.hpp"
#include "../ObjectBytes.hpp"
#include "FreeNodes.hpp"
#include "ReadStateLabelNode.hpp"


namespace FuncLib::Store
{
	namespace ObjectRelation
	{
		class ObjectRelationTree;
	}
	
	using Persistence::IReader;
	ObjectRelationTree ReadObjRelationTreeFrom(IReader auto *reader);
	void WriteObjRelationTree(ObjectRelationTree const &tree, ObjectBytes *writer);

	namespace ObjectRelation
	{
		using ::std::optional;
		using ::std::pair;

		class ObjectRelationTree : private LabelTree
		{
		private:
			friend ObjectRelationTree FuncLib::Store::ReadObjRelationTreeFrom(IReader auto *reader);
			friend void FuncLib::Store::WriteObjRelationTree(ObjectRelationTree const &tree, ObjectBytes *writer);

			using Base = LabelTree;
			FreeNodes _freeNodes;

		public:
			static void WriteObjRelationTree(ObjectRelationTree const &tree, ObjectBytes *writer);

			ObjectRelationTree(LabelTree tree, FreeNodes freeNodes = {});
			void UpdateWith(ReadStateLabelNode topNode);
			void Free(ReadStateLabelNode topNode);
			/// releaser's arg is pos_label
			void ReleaseFreeNodes(auto const &releaser)
			{
				_freeNodes.ReleaseAll(releaser);
			}

		private:
			/// complete the content from old tree to new
			void Complete(ReadStateLabelNode *newNode);
			/// Precondition: newNode->Label() == oldNode.Label()
			void Complete(ReadStateLabelNode *newNode, LabelNode oldNode);
			optional<LabelNode> Take(pos_label label);
			void Collect(LabelNode node);
		};
	}
}
