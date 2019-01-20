#pragma once
#include <memory>
#include <utility>
template <typename Key>
class Pair {
    virtual Key& key() const = 0;
    virtual void* value() const = 0;
    virtual void change_value(void*) const = 0;
    virtual ~Pair() = default;
};

template <typename Key, typename Value>
class RealPair: Pair<Key> {
    std::pair<Key, std::shared_ptr<Value>> p;
    // maybe constructor is no using
    // , because the default constructing in Node
//    explicit RealPair(const std::pair<Key, Value*>&);
public:
    Key& key() override;
    void* value() override;
    void change_value(void*) override;
    ~RealPair() override;
};
