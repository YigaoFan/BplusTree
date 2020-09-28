#include <cstddef>
#include "StaticConfig.hpp"

namespace FuncLib::Store
{
	using ::std::size_t;

	template <typename T>
	concept Writer_Write = requires(T t, char const* begin, size_t size)
	{
		t.Write(begin, size);
	};

	template <typename T>
	concept Writer_WriteBlank = requires(T t, size_t size)
	{
		t.WriteBlank(size);
	};

	template <typename T>
	concept SameToPos_int = std::is_same_v<T, pos_int>;

	template <typename T>
	concept Writer_OffsetCounter = requires(T t)
	{
		t.StartCounter();
		t.EndCounter();
		{ t.CounterNum() } -> SameToPos_int;
	};

	template <typename T>
	concept IWriter = Writer_Write<T> &&Writer_OffsetCounter<T> &&Writer_WriteBlank<T>;
}