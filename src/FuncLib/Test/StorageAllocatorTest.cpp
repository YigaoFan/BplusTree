#include "../../TestFrame/FlyTest.hpp"
#include "../Store/StorageAllocator.hpp"

using namespace FuncLib::Store;

TESTCASE("StorageAllocator test")
{
    auto filename = "storageAllocatorTest";
    auto alloca = StorageAllocator::ReadAllocatedInfoFrom(filename);
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
    StorageAllocator::WriteAllocatedInfoTo(filename, alloca);
}

DEF_TEST_FUNC(storageAllocatorTest)

