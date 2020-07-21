#pragma once
/**********************************
   Enumerator in Collections
***********************************/

#include <functional>
#include <memory>
#include "IEnumerator.hpp"
#include "../Basic/Exception.hpp"

namespace Collections
{
	using ::std::shared_ptr;
	using ::std::size_t;
	using ::std::remove_reference_t;
	using ::Basic::InvalidAccessException;

	/*template <typename Item>
	class EmptyEnumerator : public IEnumerator<Item>
	{
	public:
		Item& Current() override
		{
			throw InvalidOperationException("Can't read Current() of EmptyEnumerator");
		}
		virtual bool MoveNext() override
		{
			return false;
		}

		size_t CurrentIndex() override
		{
			throw InvalidOperationException("Can't read CurrentIndex() of EmptyEnumerator");
		}
	};*/

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

	// TODO how to remove the Iterator type in template args
	template <typename Item, typename Iterator>
	class Enumerator : public IEnumerator<Item>
	{
	private:
		bool _firstMove{ true };
		Iterator _current;
		Iterator const _begin;
		Iterator const _end;
	public:
		// TODO: support array, list, raw array?
		// TODO how to direct use constructor(arg is container) to deduce Item and Iterator type

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

		Enumerator(Iterator begin, Iterator end)
			: _current(begin), _begin(begin), _end(end)
		{ }

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
