#include "../../TestFrame/FlyTest.hpp"
#include "../Store/LabelRelationNode.hpp"

using namespace std;
using namespace FuncLib::Store;

TESTCASE("LabelRelationNode test")
{
	auto l = LabelRelationNode(0, { LabelRelationNode(1), LabelRelationNode(2), });

	SECTION("Enumerator")
	{
		auto e = l.CreateSubNodeEnumerator();
		pos_label labels[2]
		{
			1,
			2,
		};
		auto i = 0;

		while (e.MoveNext())
		{
			ASSERT(e.Current().Label() == labels[i++]);
		}
	}

	SECTION("Take")
	{
		{
			auto n = l.TakeInside(1);
			ASSERT(n.has_value());
			ASSERT(n.value().Label() == 1);
		}

		{
			auto n = l.TakeInside(2);
			ASSERT(n.has_value());
			ASSERT(n.value().Label() == 2);
		}
		
		ASSERT(l.SubsEmpty());
	}

	SECTION("Release")
	{
		l.ReleaseAll([](pos_label label) {});
		ASSERT(l.SubsEmpty());
	}
}

DEF_TEST_FUNC(TestLabelRelationNode)