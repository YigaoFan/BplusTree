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

    template <typename RawItem, typename Item, typename Iterator>
    class EnumeratorPipeine : public IEnumerator<Item>
    {
    private:
        Enumerator<RawItem, Iterator> _enumerator;
        function<Item(RawItem&)> _func;

    public:
        EnumeratorPipeine(Enumerator<RawItem, Iterator> enumerator, function<Item(RawItem&)> func)
            : _enumerator(move(enumerator)), _func(move(_func))
        { }

        Item& Current() override
        {
            return _func(_enumerator.Current());
        }

        bool MoveNext() override
        {
            return _enumerator.MoveNext();
        }

        template <typename T>
		auto operator|(function<T(Item&)> func)
        {
            return EnumeratorPipeine<RawItem, T, Iterator>(_enumerator,
                [func1 = _func, func2 = move(func)](RawItem& item) 
                {
                    func2(func1(item));
                });
        }
    };
}
