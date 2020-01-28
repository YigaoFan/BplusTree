#pragma once
/**********************************
   Enumerator in Collections
***********************************/

#include <functional>
#include <memory>
#include "IEnumerator.hpp"
#include "Exception.hpp"

namespace Collections
{
	using ::std::shared_ptr;
	using ::std::size_t;
	using ::std::remove_reference_t;

	template <typename Item>
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
	};

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

		/*Enumerator CreateNewEnumeratorByRelativeRange(int32_t end) const
		{
			return { _current, _current+end };
		}*/

		// optional<Enumerator> TryCreateNewEnumeratorByRelativeRange(int32_t end) const
		// {
		// 	if (_current+end < _end) { return GetNewEnumeratorByRelativeRange(end); }
		// 	return {};
		// }
		//
		// Enumerator CreateNewEnumeratorByRelativeRange(int32_t start, int32_t end) const
		// {
		// 	return { _current+start, _current+end };
		// }
		//
		// optional<Enumerator> TryCreateNewEnumeratorByRelativeRange(int32_t start, int32_t end) const
		// {
		// 	if (_current+start < _start || _current+end > end) { return {}; }
		// 	return GetNewEnumeratorByRelativeRange(start, end);
		// }
	};

	// template <typename Container>
	// class Enumerator<Container, true>
	// {
	// public:
	// 	// TODO
	// private:
	// 	shared_ptr<Container> _containerPtr;
	// 	Iterator _current;
	// 	Iterator _start;
	// 	Iterator _end;
	// };

	// external method to implement common part method
}
