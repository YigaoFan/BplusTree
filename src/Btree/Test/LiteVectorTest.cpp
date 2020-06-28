#include <string>
#include "../../TestFrame/FlyTest.hpp"
#include "../LiteVector.hpp"
#include "Util.hpp"

using namespace Collections;
using ::std::string;


TESTCASE("Int LiteVector test")
{
	using IntLiVec = LiteVector<int, size_t, 10>; // Just stack allocate
	auto v = IntLiVec{ 0, 1, };
	ASSERT(v.Count() == 2);

	SECTION("Default cons")
	{
		auto ev = IntLiVec();
		ASSERT(ev.Empty());
		ASSERT(!ev.Full());
	}

	SECTION("Copy cons")
	{
		auto cv = v;
		ASSERT(cv.Count() == 2);
		ASSERT(cv[0] == 0);
		ASSERT(cv[1] == 1);
	}

	SECTION("Move cons")
	{
		using ::std::move;
		auto mv = move(v);
		ASSERT(mv.Count() == 2);
		ASSERT(mv[0] == 0);
		ASSERT(mv[1] == 1);
		ASSERT(v.Empty());
	}

	SECTION("Destruct")
	{
		int a = 2;
		struct Item
		{
			int& num;
			Item(int& i) : num(i) { }
			~Item() { --num; }
		};

		LiteVector<Item, size_t, 10> items { Item(a), Item(a), };
		ASSERT(items.Count() == 2);
		// items.~LiteVector();
		// ASSERT(a == -9); // Local variable in LiteVector member function will call distructor, too
	}

	SECTION("Add")
	{
		v.Add(2);
		ASSERT(v[2] == 2);
	}

	SECTION("RemoveAt")
	{
		v.RemoveAt(0);
		ASSERT(v[0] == 1);
		ASSERT(v.Count() == 1);
	}

	SECTION("Popout related")
	{
		SECTION("PopOutItems")
		{
			auto items = v.PopOutItems(2);
			ASSERT(v.Empty());
			ASSERT(items.size() == 2);
			ASSERT(items[0] == 0);
			ASSERT(items[1] == 1);
		}

		SECTION("PopOutAll")
		{
			auto items = v.PopOutAll();
			ASSERT(v.Empty());
			ASSERT(items.size() == 2);
			ASSERT(items[0] == 0);
			ASSERT(items[1] == 1);
		}

		SECTION("FrontPopOut")
		{
			auto item = v.FrontPopOut();
			ASSERT(v.Count() == 1);
			ASSERT(v[0] == 1);
			ASSERT(item == 0);
		}

		SECTION("PopOut")
		{
			auto item = v.PopOut();
			ASSERT(v.Count() == 1);
			ASSERT(v[0] == 0);
			ASSERT(item == 1);
		}
	}

	SECTION("Emplace")
	{
		v.Emplace(1, 2);
		ASSERT(v[1] == 2);
		ASSERT(v[2] == 1);
		ASSERT(v.Count() == 3);

		v.Emplace(2, 3);
		ASSERT(v[2] == 3);
		ASSERT(v.Count() == 4);
	}

	SECTION("EmplaceHead")
	{
		v.EmplaceHead(-1);
		ASSERT(v[0] == -1);
		ASSERT(v[1] == 0);
		ASSERT(v[2] == 1);
		ASSERT(v.Count() == 3);
	}

	SECTION("Iterator")
	{
		auto i = 0;
		for (auto& e : v)
		{
			ASSERT(e == i++);
		}
	}

	SECTION("Random access")
	{
		ASSERT(v[0] == 0);
		ASSERT(v[1] == 1);
	}
}

TESTCASE("String LiteVector test")
{
	using ::std::string;
	using StrLiVec = LiteVector<string, int, 10>;
	auto v = StrLiVec{ "Hello", "World" };
	ASSERT(v.Count() == 2);
}

void liteVectorTest(bool executed)
{
	if (executed)
	{
		allTest();
	}

	_tests_.clear();
}
