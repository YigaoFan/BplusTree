#include "Data.h"
using namespace btree;
using std::make_pair;
using std::shared_ptr;

#define REALPAIR_TEMPLATE template <typename Key, typename Value>
#define REALPAIR_INSTANCE RealPair<Key, Value>

//REALPAIR_TEMPLATE
//REALPAIR_INSTANCE::RealPair(const std::pair<Key, Value*>& pair)
//: p(make_pair(pair.first, shared_ptr<Value>(pair.second)))
//{
//    // null
//}

REALPAIR_TEMPLATE
Key&
REALPAIR_INSTANCE::key()
{
    return p.first;
}

REALPAIR_TEMPLATE
void*
REALPAIR_INSTANCE::value()
{
    return p.second.get();
}

REALPAIR_TEMPLATE
void
REALPAIR_INSTANCE::change_value(void* ptr)
{
    p.second.reset(ptr);
}