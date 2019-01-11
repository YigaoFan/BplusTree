#pragma once
#include <utility> // for pair

template <typename Key>
class Feign_Ele {
public:
    // todo: complete the construct control, this and derived class
    Feign_Ele();
    ~Feign_Ele();
    virtual Key& key();
};

template <typename Key,
    typename Value>
    class Ele : private Feign_Ele<Key> {
    private:
        std::pair<Key, value_type> leaf_;
    public:
        Ele(Key, value_type);
        Key& key() override
        { return leaf_.first; }
        Value& value()
        { return leaf_.second; }
};

template<typename Key, typename Value>
Ele<Key, Value>&
make_Ele(const Key& k, const Value& v)
{
    // Obtain memory from GC
}

// todo: how to make e auto delete?
template<typename Key, typename Value>
void
del_Ele(Ele<Key, Value>& e)
{
    // Tell GC to collect this e
}

