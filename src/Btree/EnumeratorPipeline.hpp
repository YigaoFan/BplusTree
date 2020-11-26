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

    template <typename RawItem, typename Item, typename Enumerator>
    class EnumeratorPipeline
    {
    private:
        Enumerator _enumerator;
        function<Item(RawItem)> _func;

    public:
        EnumeratorPipeline(Enumerator enumerator, function<Item(RawItem)> func) requires IEnumerator<Enumerator, RawItem>
            : _enumerator(enumerator), _func(move(func))
        { }

        Item Current()
        {
            return _func(_enumerator.Current());
        }

        bool MoveNext()
        {
            return _enumerator.MoveNext();
        }

        size_t CurrentIndex()
        {
            return _enumerator.CurrentIndex();
        }

        template <typename T>
		auto operator| (function<T(Item)> func)
        {
            return EnumeratorPipeline<RawItem, T, decltype(_enumerator)>(_enumerator,
                [func1 = _func, func2 = move(func)](RawItem item) 
                {
                    return func2(move(func1(move(item))));
                });
        }
    };
}
