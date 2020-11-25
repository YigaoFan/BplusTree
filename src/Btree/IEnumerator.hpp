#pragma once
/***********************************************************************************************************
   IEnumerator interface in Collections
***********************************************************************************************************/

#include "../Basic/Concepts.hpp"

namespace Collections
{
	using Basic::IsSameTo;
	using ::std::result_of_t;
	using ::std::size_t;

	template <typename T>
	concept Enumerator_MoveNext = requires(T t)
	{
		{ t.MoveNext() } -> IsSameTo<bool>;
	};

	template <typename T, typename Value>
	concept Enumerator_Current = requires(T t)
	{
		{ t.Current() } -> IsSameTo<Value>;
	};

	template <typename T>
	concept Enumerator_CurrentIndex = requires(T t)// 在这里加模板参数，和只在上面加模板参数有什么区别
	{
		t.CurrentIndex();
	};

	template <typename T, typename Value>
	concept IEnumerator = Enumerator_MoveNext<T> and Enumerator_Current<T, Value> and Enumerator_CurrentIndex<T>;

	template <typename Enumerator>
	using ValueTypeOf = result_of_t<decltype(&Enumerator::Current)(Enumerator)>;
}
