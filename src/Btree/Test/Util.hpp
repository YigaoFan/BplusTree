#pragma once
#include <iostream>

#define LOG_HELPER(FILE, LINE, DATA) std::cout << FILE << LINE << #DATA ": " << DATA << std::endl;
#define LOG_DETAIL(DATA) LOG_HELPER(__FILE__ ": ", __LINE__" ", DATA)
#define LOG(DATA) LOG_HELPER("", "", DATA)
#define D(var) ::std::cout << #var << ": "<< var << endl

#define FOR_EACH(container, operation) \
for (auto& e : container) \
{                         \
	operation(e);         \
}

#include "../IEnumerator.hpp"
using ::Collections::IEnumerator;

// Ensure enumerator1 longer than enumerator2
template <typename T1, typename T2, typename Func>
void MapContainers(IEnumerator<T1>& enumerator1, IEnumerator<T2>& enumerator2, Func func)
{
	while (enumerator1.MoveNext() && enumerator2.MoveNext())
	{
		func(enumerator1.Current(), enumerator2.Current());
	}
}

// Ensure enumerator1 longer than enumerator2
template <typename T1, typename T2, typename Func>
void MapContainers(IEnumerator<T1>&& enumerator1, IEnumerator<T2>&& enumerator2, Func func)
{
	while (enumerator1.MoveNext() && enumerator2.MoveNext())
	{
		func(enumerator1.Current(), enumerator2.Current());
	}
}
