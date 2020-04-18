#include <string>
#include "../../TestFrame/FlyTest.hpp"
#include "../DiskPtr.hpp"
#include "../../Btree/Btree.hpp"

using namespace FuncLib;
using namespace Collections;
using namespace std;

TESTCASE("TODO")
{
	/*using Btr = Btree<4, string, string>;
	
	auto p = DiskPtr<Btr>::ReadBtreeFrom(string("FuncLibIdx"));
	auto n = p->Count();*/
	/*SECTION("TODO")
	{
	}*/
}

void diskBtreeTest(bool executed)
{
	if (executed)
	{
		allTest();
	}

	_tests_.~vector();
}
