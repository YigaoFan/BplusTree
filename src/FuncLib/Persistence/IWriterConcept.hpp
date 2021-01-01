#pragma once
#include <cstddef>
#include "../../Basic/Concepts.hpp"

namespace FuncLib::Persistence
{
	using Basic::IsSameTo;
	using ::std::size_t;

	template <typename T>
	concept Writer_Write = requires(T t, char const* begin, size_t size)
	{
		t.Add(begin, size);
	};

	template <typename T>
	concept Writer_WriteBlank = requires(T t, size_t size)
	{
		t.AddBlank(size);
	};

	// template <typename T>
	// concept Writer_OffsetCounter = requires(T t)
	// {
	// 	{ t.Size() } -> IsSameTo<size_t>;
	// };

	/// 在 ByteConverter 这里面用 IWriter，在外面可以不用，就像 ObjectBytes 和 FakeObjectBytes 在 File 里用那样
	template <typename T>
	concept IWriter = Writer_Write<T> and 
					  Writer_WriteBlank<T>;
}