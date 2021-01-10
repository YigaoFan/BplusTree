#include "../../TestFrame/FlyTest.hpp"
#define private public
#include "../Store/ObjectRelation/ObjectRelationTree.hpp"

using namespace std;
using namespace FuncLib::Store::ObjectRelation;

ObjectRelationTree GetInitTree()
{
	auto leafNode0 = LabelNode(10, {});
	auto middleNode0 = LabelNode(9, {leafNode0});
	auto leafNode1 = LabelNode(12, {});
	auto middleNode1 = LabelNode(11, {leafNode1});
	auto middleNode2 = LabelNode(8, {middleNode0, middleNode1});
	auto middleNode3 = LabelNode(2, {middleNode2});
	auto leafNode2 = LabelNode(7, {});
	auto leafNode3 = LabelNode(17, {});
	auto middleNode4 = LabelNode(1, {middleNode3, leafNode2, leafNode3});
	auto leafNode4 = LabelNode(5, {});
	auto leafNode5 = LabelNode(16, {});
	auto middleNode5 = LabelNode(15, {leafNode5});
	auto middleNode6 = LabelNode(14, {middleNode5});
	auto middleNode7 = LabelNode(6, {middleNode6});
	auto middleNode8 = LabelNode(3, {leafNode4, middleNode7});
	auto leafNode6 = LabelNode(13, {});
	auto middleNode9 = LabelNode(4, {leafNode6});
	auto middleNode10 = LabelNode(0, {middleNode4, middleNode8, middleNode9});
	return ObjectRelationTree(move(middleNode10));
}

TESTCASE("ObjectRelationTree test")
{
	auto tree = GetInitTree();

	SECTION("Pure Add")
	{
		// update part
		{
			auto leafNode0 = ReadStateLabelNode(21, {}, true);
			auto middleNode0 = ReadStateLabelNode(19, {leafNode0}, true);
			auto leafNode1 = ReadStateLabelNode(20, {}, true);
			auto middleNode1 = ReadStateLabelNode(18, {middleNode0, leafNode1}, true);
			tree.UpdateWith(middleNode1);
		}
	
		// assert tree part
		{
			auto leafNode0 = LabelNode(10, {});
			auto middleNode0 = LabelNode(9, {leafNode0});
			auto leafNode1 = LabelNode(12, {});
			auto middleNode1 = LabelNode(11, {leafNode1});
			auto middleNode2 = LabelNode(8, {middleNode0, middleNode1});
			auto middleNode3 = LabelNode(2, {middleNode2});
			auto leafNode2 = LabelNode(7, {});
			auto leafNode3 = LabelNode(17, {});
			auto middleNode4 = LabelNode(1, {middleNode3, leafNode2, leafNode3});
			auto leafNode4 = LabelNode(5, {});
			auto leafNode5 = LabelNode(16, {});
			auto middleNode5 = LabelNode(15, {leafNode5});
			auto middleNode6 = LabelNode(14, {middleNode5});
			auto middleNode7 = LabelNode(6, {middleNode6});
			auto middleNode8 = LabelNode(3, {leafNode4, middleNode7});
			auto leafNode6 = LabelNode(13, {});
			auto middleNode9 = LabelNode(4, {leafNode6});
			auto leafNode7 = LabelNode(21, {});
			auto middleNode10 = LabelNode(19, {leafNode7});
			auto leafNode8 = LabelNode(20, {});
			auto middleNode11 = LabelNode(18, {middleNode10, leafNode8});
			auto middleNode12 = LabelNode(0, {middleNode4, middleNode8, middleNode9, middleNode11});

			auto destTree = ObjectRelationTree(move(middleNode12));
			ASSERT(tree.EqualTo(destTree));
		}

		// assert free node part
		{
			auto leafNode0 = LabelNode(0, {});
			auto destFreeNodes = FreeNodes(move(leafNode0));
			ASSERT(tree._freeNodes.EqualTo(destFreeNodes));
		}
	}

	SECTION("Pure delete")
	{
		// update part
		{
			auto leafNode0 = ReadStateLabelNode(4, {}, false);
			tree.Free(leafNode0);
		}

		// assert tree part
		{
			auto leafNode0 = LabelNode(10, {});
			auto middleNode0 = LabelNode(9, {leafNode0});
			auto leafNode1 = LabelNode(12, {});
			auto middleNode1 = LabelNode(11, {leafNode1});
			auto middleNode2 = LabelNode(8, {middleNode0, middleNode1});
			auto middleNode3 = LabelNode(2, {middleNode2});
			auto leafNode2 = LabelNode(7, {});
			auto leafNode3 = LabelNode(17, {});
			auto middleNode4 = LabelNode(1, {middleNode3, leafNode2, leafNode3});
			auto leafNode4 = LabelNode(5, {});
			auto leafNode5 = LabelNode(16, {});
			auto middleNode5 = LabelNode(15, {leafNode5});
			auto middleNode6 = LabelNode(14, {middleNode5});
			auto middleNode7 = LabelNode(6, {middleNode6});
			auto middleNode8 = LabelNode(3, {leafNode4, middleNode7});
			auto middleNode9 = LabelNode(0, {middleNode4, middleNode8});

			auto destTree = ObjectRelationTree(move(middleNode9));
			ASSERT(tree.EqualTo(destTree));
		}

		// assert free node part
		{
			auto leafNode0 = LabelNode(13, {});
			auto middleNode0 = LabelNode(4, {leafNode0});
			auto middleNode1 = LabelNode(0, {middleNode0});

			auto destFreeNodes = FreeNodes(move(middleNode1));
			ASSERT(tree._freeNodes.EqualTo(destFreeNodes));
		}
	}

	SECTION("Move")
	{
		// update part
		{
			auto leafNode0 = ReadStateLabelNode(14, {}, false);
			auto middleNode0 = ReadStateLabelNode(13, {leafNode0}, true);
			auto middleNode1 = ReadStateLabelNode(4, {middleNode0}, true);
			tree.UpdateWith(middleNode1);
		}

		// assert tree part
		{
			auto leafNode0 = LabelNode(10, {});
			auto middleNode0 = LabelNode(9, {leafNode0});
			auto leafNode1 = LabelNode(12, {});
			auto middleNode1 = LabelNode(11, {leafNode1});
			auto middleNode2 = LabelNode(8, {middleNode0, middleNode1});
			auto middleNode3 = LabelNode(2, {middleNode2});
			auto leafNode2 = LabelNode(7, {});
			auto leafNode3 = LabelNode(17, {});
			auto middleNode4 = LabelNode(1, {middleNode3, leafNode2, leafNode3});
			auto leafNode4 = LabelNode(5, {});
			auto leafNode5 = LabelNode(6, {});
			auto middleNode5 = LabelNode(3, {leafNode4, leafNode5});
			auto leafNode6 = LabelNode(16, {});
			auto middleNode6 = LabelNode(15, {leafNode6});
			auto middleNode7 = LabelNode(14, {middleNode6});
			auto middleNode8 = LabelNode(13, {middleNode7});
			auto middleNode9 = LabelNode(4, {middleNode8});
			auto middleNode10 = LabelNode(0, {middleNode4, middleNode5, middleNode9});

			auto destTree = ObjectRelationTree(move(middleNode10));
			ASSERT(tree.EqualTo(destTree));
		}

		// assert free node part
		{
			auto leafNode0 = LabelNode(0, {});
			auto destFreeNodes = FreeNodes(move(leafNode0));
			ASSERT(tree._freeNodes.EqualTo(destFreeNodes));
		}
	}

	SECTION("Add and Move")
	{
		// update part
		{
			auto leafNode0 = ReadStateLabelNode(11, {}, false);
			auto leafNode1 = ReadStateLabelNode(19, {}, true);
			auto middleNode0 = ReadStateLabelNode(18, {leafNode0, leafNode1}, true);
			auto middleNode1 = ReadStateLabelNode(13, {middleNode0}, true);
			auto middleNode2 = ReadStateLabelNode(4, {middleNode1}, true);
			tree.UpdateWith(middleNode2);
		}

		// assert tree part
		{
			auto leafNode0 = LabelNode(10, {});
			auto middleNode0 = LabelNode(9, {leafNode0});
			auto middleNode1 = LabelNode(8, {middleNode0});
			auto middleNode2 = LabelNode(2, {middleNode1});
			auto leafNode1 = LabelNode(7, {});
			auto leafNode2 = LabelNode(17, {});
			auto middleNode3 = LabelNode(1, {middleNode2, leafNode1, leafNode2});
			auto leafNode3 = LabelNode(5, {});
			auto leafNode4 = LabelNode(16, {});
			auto middleNode4 = LabelNode(15, {leafNode4});
			auto middleNode5 = LabelNode(14, {middleNode4});
			auto middleNode6 = LabelNode(6, {middleNode5});
			auto middleNode7 = LabelNode(3, {leafNode3, middleNode6});
			auto leafNode5 = LabelNode(12, {});
			auto middleNode8 = LabelNode(11, {leafNode5});
			auto leafNode6 = LabelNode(19, {});
			auto middleNode9 = LabelNode(18, {middleNode8, leafNode6});
			auto middleNode10 = LabelNode(13, {middleNode9});
			auto middleNode11 = LabelNode(4, {middleNode10});
			auto middleNode12 = LabelNode(0, {middleNode3, middleNode7, middleNode11});

			auto destTree = ObjectRelationTree(move(middleNode12));
			ASSERT(tree.EqualTo(destTree));
		}

		// assert free node part
		{
			auto leafNode0 = LabelNode(0, {});
			auto destFreeNodes = FreeNodes(move(leafNode0));
			ASSERT(tree._freeNodes.EqualTo(destFreeNodes));
		}
	}

	SECTION("Move and Delete")
	{
		// update part
		{
			auto leafNode0 = ReadStateLabelNode(8, {}, false);
			auto middleNode0 = ReadStateLabelNode(13, {leafNode0}, true);
			auto middleNode1 = ReadStateLabelNode(4, {middleNode0}, true);
			tree.Free(middleNode1);
		}

		// assert tree part
		{
			auto leafNode0 = LabelNode(2, {});
			auto leafNode1 = LabelNode(7, {});
			auto leafNode2 = LabelNode(17, {});
			auto middleNode0 = LabelNode(1, {leafNode0, leafNode1, leafNode2});
			auto leafNode3 = LabelNode(5, {});
			auto leafNode4 = LabelNode(16, {});
			auto middleNode1 = LabelNode(15, {leafNode4});
			auto middleNode2 = LabelNode(14, {middleNode1});
			auto middleNode3 = LabelNode(6, {middleNode2});
			auto middleNode4 = LabelNode(3, {leafNode3, middleNode3});
			auto middleNode5 = LabelNode(0, {middleNode0, middleNode4});

			auto destTree = ObjectRelationTree(move(middleNode5));
			ASSERT(tree.EqualTo(destTree));
		}

		// assert free node part
		{
			auto leafNode0 = LabelNode(10, {});
			auto middleNode0 = LabelNode(9, {leafNode0});
			auto leafNode1 = LabelNode(12, {});
			auto middleNode1 = LabelNode(11, {leafNode1});
			auto middleNode2 = LabelNode(8, {middleNode0, middleNode1});
			auto middleNode3 = LabelNode(13, {middleNode2});
			auto middleNode4 = LabelNode(4, {middleNode3});
			auto middleNode5 = LabelNode(0, {middleNode4});

			auto destFreeNodes = FreeNodes(move(middleNode5));
			ASSERT(tree._freeNodes.EqualTo(destFreeNodes));
		}
	}
}

DEF_TEST_FUNC(TestObjectRelationTree)