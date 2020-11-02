#pragma once
/**********************************
   Enumerator in Collections
***********************************/

#include "IEnumerator.hpp"
#include "../Basic/Exception.hpp"

namespace Collections
{
	using ::Basic::InvalidAccessException;
	using ::std::size_t;

	template <typename Item, typename Iterator>
	class Enumerator;

	template <typename Container>
	static
	auto
	CreateEnumerator(Container& container) -> Enumerator<decltype(*container.begin()), decltype(container.begin())>
	{
		return { container.begin(), container.end() };
	}

	template <typename Iterator>
	static
	auto
	CreateEnumerator(Iterator begin, Iterator end) -> Enumerator<decltype(*begin), Iterator>
	{
		return { begin, end };
	}

	template <typename Item, typename Iterator>
	class Enumerator : public IEnumerator<Item>
	{
	private:
		bool _firstMove{ true };
		Iterator _current;
		Iterator const _begin;
		Iterator const _end;
	public:
		Enumerator(Iterator begin, Iterator end) : _current(begin), _begin(begin), _end(end)
		{ }

		Item Current() override
		{
			if (_firstMove) 
			{
				throw InvalidAccessException();
			}

			return *_current;
		}

		size_t CurrentIndex() override
		{
			return _current - _begin;
		}

		bool MoveNext() override
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
