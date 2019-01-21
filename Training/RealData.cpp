#include "RealData.h"
using namespace btree;

#define REALDATA_TEMPLATE template <typename Key, typename Value>
#define REALDATA_INSTANCE RealData<Key, Value>

REALDATA_TEMPLATE
REALDATA_INSTANCE::RealData(const Value& value)
    : pair_(new Value(value))
{
    // null
}

REALDATA_TEMPLATE
void*
REALDATA_INSTANCE::value() const
{
    return &(pair_.second);
}

REALDATA_TEMPLATE
void
REALDATA_INSTANCE::change_value(void* ptr)
{
    // maybe construct directly not change
    // TODO: how to delete needless memory
    pair_.reset(ptr);
}