#include <cstddef>

namespace FuncLib::Store
{
	using ::std::size_t;

	template <typename T>
	concept Writer_Write = requires(T t, char const* begin, size_t size)
	{
		t.Write(begin, size);
	};

	template <typename T>
	concept Writer_CurrentPos = requires(T t)
	{
		t.CurrentPos();
	};


	template <typename T>
	concept IWriter = Writer_Write<T> && Writer_CurrentPos<T>;
}