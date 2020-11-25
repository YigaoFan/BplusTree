#pragma once
/**********************************
   Enumerator in Collections
***********************************/

#include <type_traits>
#include "IEnumerator.hpp"
#include "../Basic/Exception.hpp"

namespace Collections
{
	using ::Basic::InvalidAccessException;
	using ::std::is_reference_v;
	using ::std::remove_reference_t;
	using ::std::size_t;

	template <typename Item, typename Iterator>
	class Enumerator;

	template <typename Container>
	static
	auto
	CreateRefEnumerator(Container& container) -> Enumerator<decltype(*container.begin()), decltype(container.begin())>
	{
		return { container.begin(), container.end() };
	}

	template <typename Iterator>
	static
	auto
	CreateRefEnumerator(Iterator begin, Iterator end) -> Enumerator<decltype(*begin), Iterator>
	{
		return { begin, end };
	}

	template <typename Container>
	static auto
	CreateMoveEnumerator(Container &container) -> Enumerator<remove_reference_t<decltype(*container.begin())>, decltype(container.begin())>
	{
		return { container.begin(), container.end() };
	}

	template <typename Iterator>
	static auto
	CreateMoveEnumerator(Iterator begin, Iterator end) -> Enumerator<remove_reference_t<decltype(*begin)>, Iterator>
	{
		return { begin, end };
	}

	template <typename Item, typename Iterator>
	class Enumerator
	{
	private:
		bool _firstMove{ true };
		Iterator _current;
		Iterator const _begin;
		Iterator const _end;
	public:
		Enumerator(Iterator begin, Iterator end) : _current(begin), _begin(begin), _end(end)
		{ }

		Item Current()
		{
			if (_firstMove) 
			{
				throw InvalidAccessException();
			}

			if constexpr (is_reference_v<Item>)
			{
				return *_current;
			}
			else
			{
				return move(*_current);
			}
		}

		size_t CurrentIndex()
		{
			return _current - _begin;
		}

		bool MoveNext()
		{
			if (_begin == _end)
			{
				return false;
			}

			if (_firstMove)
			{
				_firstMove = false;
				return true;
			}

			++_current;
			if (_current != _end)
			{
				return true;
			}

			return false;
		}
	};
}
