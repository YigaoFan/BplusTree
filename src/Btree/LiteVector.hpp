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
	using ::std::remove_const_t;

	template <typename T, typename size_int, size_int Capacity>
	class LiteVector
	{
	private:
		array<uint8_t, sizeof(T) * Capacity> _mem;
		T* const _ptr = reinterpret_cast<T*>(&_mem[0]);
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
			while (_count != 0)
			{
				auto& e = _ptr[--_count];
				e.~T();
			}
		}

		void Add(T t)
		{
			new (_ptr + (_count++))T(move(t));
		}

		void RemoveAt(size_int i)
		{
			MoveLeft<true>(i);
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
			return move(_ptr[--_count]);
		}

		T FrontPopOut()
		{
			auto p = move(_ptr[0]);
			MoveLeft<false>(0);
			--_count;
			return move(p);
		}

		void Emplace(size_int i, T t)
		{
			// TODO back 会不会不对？
			MoveRight(i);
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
		T& FirstOne() { return _ptr[0]; }
		T& LastOne() { return _ptr[_count - 1]; }
		T const& FirstOne() const { return _ptr[0]; }
		T const& LastOne() const { return _ptr[_count - 1]; }

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

		/// i item will be covered
		template <bool NeedToDestroy>
		void MoveLeft(size_int i)
		{
			if constexpr (NeedToDestroy)
			{
				_ptr[i].~T();
			}

			remove_const_t<decltype(_ptr)> start = begin() + i;
			new (start)T(move(*(start + 1)));
			++start;
			for (; start != (this->end() - 1); ++start)
			{
				*(start) = move(*(start + 1));
			}
		}

		void MoveRight(size_int index)
		{
			decltype(_ptr) start = begin() + index;
			auto rbegin = this->end() - 1;
			new (this->end())T(move(*rbegin));
			--rbegin;
			for (; rbegin != (start - 1); --rbegin)
			{
				*(rbegin + 1) = move(*rbegin);
			}
		}
	};
}