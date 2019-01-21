#pragma once
#include <memory>
#include <utility>
namespace btree {
    template <typename Key>
    class Data {
        virtual Key& key() const = 0;
        virtual void* value() const = 0;
        virtual void change_value(void*) const = 0;
        virtual ~Data() = default;
    };

    template <typename Key, typename Value>
    class RealPair : Data<Key> {
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
}
