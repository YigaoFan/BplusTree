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
    auto reader  = FileReader::MakeReader(nullptr, MakeFilePath(filename), 0 );
    auto alloca = StorageAllocator::ReadAllocatedInfoFrom(&reader);
    auto label = alloca.AllocatePosLabel();
    ASSERT(!alloca.Ready(label));
    auto size = 1;
    auto pos = alloca.GiveSpaceTo(label, size);
    ASSERT(pos == alloca.GetConcretePos(label));
    ASSERT(size == alloca.GetAllocatedSize(label));
    auto newSize = size + 1;
    auto newPos = alloca.ResizeSpaceTo(label, newSize);
    ASSERT(newPos == alloca.GetConcretePos(label));
    ASSERT(newSize == alloca.GetAllocatedSize(label));

    alloca.DeallocatePosLabel(label);
    ObjectBytes bytes{ 0 };
    StorageAllocator::WriteAllocatedInfoTo(alloca, &bytes);
}

DEF_TEST_FUNC(storageAllocatorTest)

