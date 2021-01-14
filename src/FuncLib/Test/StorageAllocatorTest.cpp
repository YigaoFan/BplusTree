#include "../../TestFrame/FlyTest.hpp"
#define private public
#include "../Store/StorageAllocator.hpp"
#include "StringReader.hpp"
#include "../Store/ObjectBytes.hpp"
#include "../Store/StoreInfoPersistence.hpp"
#include "Util.hpp"

using namespace FuncLib::Store;
using namespace FuncLib::Test;

TESTCASE("StorageAllocator test")
{
    SECTION("Common function")
    {
        auto alloca = StorageAllocator();
        auto label = alloca.AllocatePosLabel();
        ASSERT(!alloca.Ready(label));
        auto size = 1;
        alloca.GiveSpaceTo(label, size);
        ASSERT(size == alloca.GetAllocatedSize(label));
        auto newSize = size + 1;
        alloca.ResizeSpaceTo(label, newSize);
        ASSERT(newSize == alloca.GetAllocatedSize(label));

        // alloca.DeallocatePosLabel(label);

        SECTION("Persistence")
        {
            ObjectBytes writer{0};
            WriteAllocatedInfoTo(alloca, &writer);
            string bs;
            writer.WriteIn([&bs](vector<char> const *bytes) 
            {
                bs.append(bytes->begin(), bytes->end());
            });
            StringReader reader(move(bs));
            auto copyAllocator = ReadAllocatedInfoFrom(&reader);
            ASSERT(copyAllocator._currentPos == alloca._currentPos);
            ASSERT(copyAllocator._currentLabel == alloca._currentLabel);
            ASSERT(copyAllocator._deletedLabelTable == alloca._deletedLabelTable);
            ASSERT(copyAllocator._usingLabelTable == alloca._usingLabelTable);
        }
    }

}

DEF_TEST_FUNC(TestStorageAllocator)

