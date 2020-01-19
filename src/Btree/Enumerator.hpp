#pragma once
/**********************************
   Enumerator in Collections
***********************************/

#include <functional>
#include <optional>
#include <memory>

namespace Collections
{
	using ::std::optional;
	using ::std::shared_ptr;
	using ::std::decay_t;

	template <typename Item, typename Iterator>
	class Enumerator;

	template <typename Container>
	static
	Enumerator<typename Container::value_type, typename Container::iterator>
	CreateEnumerator(Container& container)
	{
		return { container.begin(), container.end() };
	}

	template <typename Iter>
	static
	auto
	CreateEnumerator(Iter begin, Iter end) -> Enumerator<decay_t<decltype(*begin)>, Iter>
	{
		return { begin, end };
	}

	template <typename Item, typename Iterator>
	class Enumerator
	{
	private:
		Iterator _current;
		Iterator _begin;
		Iterator _end;
	public:
		using ValueType = Item;
		// TODO: support array, list, raw array?
		// TODO how to direct use constructor(arg is container) to deduce Item and Iterator type


		Item& Current()
		{
			return *_current;
		}

		Enumerator(Iterator begin, Iterator end)
			: _begin(begin), _end(end)
		{ }

		bool MoveNext()
		{
			if (_current != _end)
			{
				++_current;
				return true;
			}

			return false;
		}

		Enumerator CreateNewEnumeratorByRelativeRange(int32_t end) const
		{
			return { _current, _current+end };
		}

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
