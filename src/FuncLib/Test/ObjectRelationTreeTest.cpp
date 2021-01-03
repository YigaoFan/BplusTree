#include "../../TestFrame/FlyTest.hpp"
#include "../Store/ObjectRelationTree.hpp"

using namespace std;
using namespace FuncLib::Store;

LabelNode GenNew()
{
	auto leafNode0 = LabelNode(1);
	auto middleNode0 = LabelNode(0, {leafNode0});
	return middleNode0;
}

TESTCASE("ObjectRelationTree test")
{
	auto l = LabelNode(0, { LabelNode(1), LabelNode(2), });
	auto leafNode0 = LabelNode(6);
	auto leafNode1 = LabelNode(7);
	auto middleNode0 = LabelNode(5, {leafNode0, leafNode1});
	auto leafNode2 = LabelNode(7);
	auto leafNode3 = LabelNode(8);
	auto middleNode1 = LabelNode(6, {leafNode2, leafNode3});
	auto middleNode2 = LabelNode(4, {middleNode0, middleNode1});
	auto middleNode3 = LabelNode(3, {middleNode2});
	auto leafNode4 = LabelNode(4);
	auto middleNode4 = LabelNode(2, {middleNode3, leafNode4});
	auto leafNode5 = LabelNode(3);
	auto middleNode5 = LabelNode(1, {middleNode4, leafNode5});
	auto leafNode6 = LabelNode(6);
	auto leafNode7 = LabelNode(8);
	auto middleNode6 = LabelNode(7, {leafNode7});
	auto middleNode7 = LabelNode(5, {leafNode6, middleNode6});
	auto middleNode8 = LabelNode(4, {middleNode7});
	auto middleNode9 = LabelNode(3, {middleNode8});
	auto middleNode10 = LabelNode(2, {middleNode9});
	auto leafNode8 = LabelNode(6);
	auto middleNode11 = LabelNode(5, {leafNode8});
	auto middleNode12 = LabelNode(4, {middleNode11});
	auto middleNode13 = LabelNode(3, {middleNode12});

	auto middleNode14 = LabelNode(0, {middleNode5, middleNode10, middleNode13});

	// auto tree = ObjectRelationTree(middleNode14);
	auto newObjRelation = GenNew();
	// tree.UpdateWith(&newObjRelation);
	// 如何把运行后的结果表示出来 TODO
	// 先把那边的代码看一遍吧
	// 或者 ASSERT 验证代码也生成出来
}

DEF_TEST_FUNC(TestObjectRelationTree)