#include <memory>
#include "../../TestFrame/FlyTest.hpp"
#define private public
#include "../Store/FileCache.hpp"
#include "Util.hpp"

using namespace FuncLib::Store;
using namespace FuncLib::Test;
using namespace std;

TESTCASE("FileCache test")
{
	auto fileId = 1;
	auto cache = FileCache(fileId);

	SECTION("Basic function")
	{
		auto posLabel = 0;// 其实这里是某种唯一标注的数字就可以，不用特意是 pos_label
		using T = int;
		auto obj = make_shared<T>(1);

		ASSERT(not cache.Cached<T>(posLabel));
		cache.RegisterSetter<T>(posLabel, [](T* n) -> void
		{
			*n = 10;
		});
		cache.RegisterSetter<T>(posLabel, [](T* n) -> void
		{
			*n = 20;
		});
		ASSERT(1 == *obj);
		ASSERT(std::holds_alternative<FileCache::SettersOf<T>>(FileCache::Cache<T>[fileId][posLabel]));
		cache.Add(posLabel, obj);

		ASSERT(cache.Cached<T>(posLabel));
		ASSERT(obj == cache.Read<T>(posLabel));
		ASSERT(20 == *cache.Read<T>(posLabel));
		cache.Remove<T>(posLabel);
		ASSERT(not cache.Cached<T>(posLabel));
	}
}

DEF_TEST_FUNC(TestFileCache)