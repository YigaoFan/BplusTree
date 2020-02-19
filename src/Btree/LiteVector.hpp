#pragma once
#include <cstddef>
#include <cstdint>
#include <vector>
#include <array>

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
		T* const _ptr = reinterpret_cast<T*>(&_mem[0]);
	protected:
		size_int _count = 0;

	public:
		using Item = T;
		
		LiteVector() { }

		template <typename... Ts>
		LiteVector(T t, Ts... ts)
		{
			Init(move(t), move(ts)...);
		}

		LiteVector(LiteVector const& that)
		{
			for (auto& i : that)
			{
				Add(i);
				//_ptr[_count++] = i;
				// difference to up: this is assign.
				// Maybe need to destruct left value first(but must some operation), then copy
				// but here memory is raw, error
			}
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
			outItems.reserve(count);
			for (decltype(_count) i = _count - count; i < _count; ++i)
			{
				outItems.push_back(move(_ptr[i]));
			}

			_count -= count;
			return outItems;
		}

		vector<T> PopOutAll()
		{
			return PopOutItems(_count);
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
			MoveItems(1, i);
			_ptr[i] = move(t);
			++_count;
		}

		void EmplaceHead(T t)
		{
			Emplace(0, move(t));
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
			if constexpr (sizeof...(ts) > 0)
			{
				Init(move(ts)...);
			}
		}

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
				auto rend = start - 1;
				for (auto rbegin = this->end() - 1; rbegin != rend; --rbegin)
				{
					*(rbegin + direction) = move(*rbegin);
				}
			}
		}
	};
}