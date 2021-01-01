#include "../../TestFrame/FlyTest.hpp"
#include "../Store/LabelNode.hpp"

using namespace std;
using namespace FuncLib::Store;

TESTCASE("LabelNode test")
{
	auto l = LabelNode(0, { LabelNode(1), LabelNode(2), });

	SECTION("Enumerator")
	{
		auto e = l.CreateSortedSubNodeEnumerator();
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

	SECTION("Add sub")
	{
		l.AddSub(LabelNode(0));
		auto e = l.CreateSortedSubNodeEnumerator();
		pos_label labels[3]
		{
			0,
			1,
			2,
		};
		auto i = 0;

		while (e.MoveNext())
		{
			ASSERT(e.Current().Label() == labels[i++]);
		}
	}
}

DEF_TEST_FUNC(TestLabelNode)