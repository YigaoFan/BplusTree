#pragma once
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace Collections
{
	using ::std::size_t;
	using ::std::uint8_t;
	using ::std::memcpy;
	using ::std::move;

	template <typename T, size_t Capacity>
	class LiteVector
	{
	private:
		// alignas how to use like array type? the T is Item type
		alignas(T) uint8_t _mem[sizeof(T)];
		T* _ptr = reinterpret_cast<T*>(_mem);
		size_t _count = 0;

	public:
		template <typename... Ts>
		LiteVector(Ts... ts)
		{
			Init(ts...); // how to move this ts...
		}

		LiteVector()
		{ }

		void Add(T t)
		{
			new (_ptr[_count++])(move(t));
		}

		~LiteVector()
		{
			for (decltype(_count) i = 0; i < size_t; ++i)
			{
				_ptr[i]->~T();
			}
		}

	private:
		template <typename... Ts>
		void Init(T t, Ts... ts)
		{
			Add(move(t));
			if (sizeof...(ts) > 0)
			{
				Init(ts...);
			}
		}
	};
}