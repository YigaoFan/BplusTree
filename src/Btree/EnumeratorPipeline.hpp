#pragma once
/**********************************
   EnumeratorPipeline in Collections
***********************************/

#include <functional>
#include <memory>
#include "IEnumerator.hpp"
#include "Enumerator.hpp"


namespace Collections
{
    using ::std::function;
    using ::std::move;
    using ::std::size_t;

    // TODO could refactor function process related code
    template <typename RawItem, typename Item> // TODO add another type arg for func arg
    class EnumeratorPipeline : public IEnumerator<Item>
    {
    private:
        IEnumerator<RawItem>& _enumerator;
        function<Item(RawItem)> _func;

    public:
        EnumeratorPipeline(IEnumerator<RawItem>& enumerator, function<Item(RawItem)> func)
            : _enumerator(enumerator), _func(move(func))
        { }

        EnumeratorPipeline(IEnumerator<RawItem>&& enumerator, function<Item(RawItem)> func)
            : _enumerator(enumerator), _func(move(func))
        { }

        Item Current() override
        {
            return _func(_enumerator.Current());
        }

        bool MoveNext() override
        {
            return _enumerator.MoveNext();
        }

        size_t CurrentIndex() override
        {
            return _enumerator.CurrentIndex();
        }

        template <typename T>
		auto operator| (function<T(Item&)> func)
        {
            return EnumeratorPipeline<RawItem, T>(_enumerator,
                [func1 = _func, func2 = move(func)](RawItem item) 
                {
                    return func2(move(func1(move(item))));
                });
        }
    };
}
