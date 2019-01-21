#pragma once
namespace btree {
    // Data use to a interface file, so constructor is protected
    class Data {
    public:
        virtual void* value() const = 0;
        virtual void change_value(void*) = 0;
        virtual ~Data() = default;
    protected:
        Data() = default;
    };
}
