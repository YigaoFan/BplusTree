#pragma once
#include <cstddef>
#include <cstdint>
#include <vector>
#include <array>
#include <type_traits>

namespace Collections
{
	using ::std::size_t;
	using ::std::uint8_t;
	using ::std::move;
	using ::std::vector;
	using ::std::array;

	template <typename T, typename size_int, size_int Capacity>
	class LiteVector
	{
	private:
		array<uint8_t, sizeof(T) * Capacity> _mem;
		T* _ptr = reinterpret_cast<T*>(&_mem[0]);
	protected:
		size_int _count = 0;

	public:
		LiteVector() { }

		template <typename... Ts>
		LiteVector(T t, Ts... ts)
		{
			Init(move(t), move(ts)...);
		}

		LiteVector(LiteVector const& that)
			: _mem(that._mem), _count(that._count)
		{ 
			static_assert(::std::is_copy_constructible_v<T>);
		}

		LiteVector(LiteVector&& that)
			: _mem(move(that._mem)), _count(that._count)
		{ 
			that._count = 0;
		}

		virtual ~LiteVector()
		{
			for (size_int i = 0; i < _count; ++i)
			{
				_ptr[i].~T();
			}
		}

		void Add(T t)
		{
			new (_ptr + (_count++))T(move(t));
		}

		void RemoveAt(size_int i)
		{
			MoveItems(-1, i + 1);
			--_count;
		}

		vector<T> PopOutItems(size_int count)
		{
			vector<T> outItems;
			for (;count != 0; --count)
			{
				outItems.push_back(move(_ptr[_count - 1]));
				--_count;
			}

			return outItems;
		}

		T PopOut()
		{
			return move(_ptr[(_count--) - 1]);
		}

		T FrontPopOut()
		{
			auto p = move(_ptr[0]);
			RemoveAt(0);
			return move(p);
		}

		void Emplace(size_int i, T t)
		{
			MoveItems(1, i + 1);
			_ptr[i + 1] = move(t);
			++_count;
		}

		T& operator[] (size_int i) { return _ptr[i]; }
		T const& operator[] (size_int i) const { return _ptr[i]; }
		T* begin() { return _ptr; }
		T* end() { return _ptr + _count; }
		T const* begin() const { return _ptr; }
		T const* end() const { return _ptr + _count; }

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

	protected:
		void MoveItems(int32_t direction, size_int index)
		{
			MoveItems(direction, begin() + index);
		}

		/// Start included, still exist
		/// \param direction
		/// \param start
		void MoveItems(int32_t direction, decltype(_ptr) start)
		{
			if (direction == 0) { return; }
			if (direction < 0)
			{
				auto e = this->end();
				for (auto begin = start; begin != e; ++begin)
				{
					*(begin + direction) = move(*begin);
				}
			}
			else
			{
				auto rend =  start - 1;
				for (auto rbegin = _ptr + _count - 1; rbegin != rend; --rbegin)
				{
					*(rbegin + direction) = move(*rbegin);
				}
			}
		}
	};
}