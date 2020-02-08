#pragma once
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <vector>

namespace Collections
{
	using ::std::size_t;
	using ::std::uint8_t;
	using ::std::memcpy;
	using ::std::move;
	using ::std::vector;

	// TODO the below grammar is correct?
	template <typename T, typename size_int, size_int Capacity>
	class LiteVector
	{
	private:
		alignas(T) uint8_t _mem[sizeof(T)];
		T* _ptr = reinterpret_cast<T*>(_mem);
		size_int _count = 0;

	public:
		template <typename... Ts>
		LiteVector(Ts... ts)
		{
			Init(move(ts)...);
		}

		LiteVector()
		{ }

		virtual ~LiteVector()
		{
			for (decltype(_count) i = 0; i < size_int; ++i)
			{
				_ptr[i]->~T();
			}
		}

		void Add(T t)
		{
			new (_ptr[_count++])(move(t));
		}

		void RemoveAt(size_int i)
		{

		}

		vector<T> PopOutItems(size_int count)
		{

		}

		T FrontPopOut()
		{

		}

		void Emplace(size_int i, T t)
		{

		}

		T& operator[] (size_int i) { return *_ptr[i]; }
		T const& operator[] (size_int i) const { return *_ptr[i]; }
		auto begin() { return _ptr; }
		auto end() { return _ptr[_count]; }
		auto begin() const { return _ptr; }
		auto end() const { return _ptr[_count]; }

		size_int Count() const { return _count; }
		bool Full() const { return _count == Capacity; }
		bool Empty() const { return _count == 0; }

	private:
		template <typename... Ts>
		void Init(T t, Ts... ts)
		{
			Add(move(t));
			if (sizeof...(ts) > 0)
			{
				Init(move(ts)...);
			}
		}
	};
}