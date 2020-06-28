#include <string>
#include <array>
#include <utility>
#include <iostream>
#include <memory>
#include "../../TestFrame/FlyTest.hpp"
#include "../Elements.hpp"
#include "../Enumerator.hpp"
using namespace Collections;
using namespace ::std;

TESTCASE("Element test")
{
	auto lessThan = [](int const& a, int const& b) { return a < b; };
	auto lessThanPtr = make_shared<function<bool(int const&, int const&)>>(lessThan);

	auto kv0 = make_pair<int, string>(0, "Hello world, fanyigao");
	auto kv1 = make_pair<int, string>(1, "Hello world, fanyigao");
	auto kv2 = make_pair<int, string>(2, "Hello world, fanyigao");
	auto kv3 = make_pair<int, string>(3, "Hello world, fanyigao");
	auto kv4 = make_pair<int, string>(4, "Hello world, fanyigao");
	auto kv5 = make_pair<int, string>(5, "Hello world, fanyigao");
	auto kv6 = make_pair<int, string>(6, "Hello world, fanyigao");
	auto kv7 = make_pair<int, string>(7, "Hello world, fanyigao");
	auto kv8 = make_pair<int, string>(8, "Hello world, fanyigao");
	auto kv9 = make_pair<int, string>(9, "Hello world, fanyigao");

	auto kvs = array<decltype(kv0), 4>
	{
		kv0,
		kv1,
		kv2,
		kv3,
	};

	using Ele = Elements<int, string, 4>;

	SECTION("Enumerator cons")
	{
		auto es = Ele(CreateEnumerator(kvs), lessThanPtr);
		ASSERT(es[0].first == kv0.first);
		ASSERT(es[1].first == kv1.first);
		ASSERT(es[2].first == kv2.first);
		ASSERT(es[3].first == kv3.first);
		ASSERT(es.Count() == 4);
	}

	SECTION("Empty cons")
	{
		auto es = Ele(lessThanPtr);
		for (auto& kv : kvs)
		{
			es.Add(kv);
			ASSERT(es.ContainsKey(kv.first));
			ASSERT(es.GetValue(kv.first) == kv.second);
			ASSERT(es[es.Count() - 1].first == kv.first);
		}
	}

	SECTION("Member function")
	{
		auto es = Ele(CreateEnumerator(kvs), lessThanPtr);

		SECTION("Move cons")
		{
		}

		SECTION("Copy cons")
		{
		}

		SECTION("ExchangeMax")
		{
		}

		SECTION("ExchangeMin")
		{
		}

		SECTION("IndexKeyOf")
		{
		}

		SECTION("SelectBranch")
		{
		}

		SECTION("GetEnumerator")
		{
		}
	}
}

DEF_TEST_FUNC(elementsTest)
