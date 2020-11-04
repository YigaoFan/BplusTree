#include "../../TestFrame/FlyTest.hpp"
#include "../Store/StorageAllocator.hpp"
#include "../Store/FileReader.hpp"
#include "../Store/ObjectBytes.hpp"
#include "Util.hpp"

using namespace FuncLib::Store;
using namespace FuncLib::Test;

TESTCASE("StorageAllocator test")
{
    auto filename = "storageAllocatorTest";
    FileReader reader{ MakeFilePath(filename), 0 };
    auto alloca = StorageAllocator::ReadAllocatedInfoFrom(&reader);
    auto lable = alloca.AllocatePosLable();
    ASSERT(!alloca.Ready(lable));
    auto size = 1;
    auto pos = alloca.GiveSpaceTo(lable, size);
    ASSERT(pos == alloca.GetConcretePos(lable));
    ASSERT(size == alloca.GetAllocatedSize(lable));
    auto newSize = size + 1;
    auto newPos = alloca.ResizeSpaceTo(lable, newSize);
    ASSERT(newPos == alloca.GetConcretePos(lable));
    ASSERT(newSize == alloca.GetAllocatedSize(lable));

    alloca.DeallocatePosLable(lable);
    ObjectBytes bytes{ 0 };
    StorageAllocator::WriteAllocatedInfoTo(alloca, &bytes);
}

DEF_TEST_FUNC(storageAllocatorTest)

