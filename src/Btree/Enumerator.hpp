#pragma once
/**********************************
   Enumerator in Util
***********************************/

#include <functional>
#include <optional>
#include <memory>

namespace Util {
	using ::std::ref;
	using ::std::move;
	using ::std::optional;
	using ::std::shared_ptr;

	/**
	 * Enumerator is just like iterator, will not copy
	 */
	template <typename Container, bool OutOfScopeUse=false>
	class Enumerator {
	private:
		using T = Container::value_type; // should use a type trait to support some type like raw array
		using Iterator = Container::iterator; // TODO have problem
	public:
		ref<T> Current;

		// TODO: support array, list, raw array? 
		Enumerator(Container& container)
			: _current(container.begin()), _start(_current), _end(container.end())
		{ }

		Enumerator(const char* container)
			: _current(container.begin()), _start(_current), _end(container.end())
		{ }

		Enumerator(Iterator start, Iterator end)
			: _current(move(start)), _start(_current), _end(move(end))
		{ }

		bool MoveNext() const
		{
			if (_current != _end) {
				Current = ref(*(++_current));
				return true;
			}

			return false;
		}

		Enumerator CreateNewEnumeratorByRelativeRange(int32_t end) const
		{
			// TODO not very familar with this init way
			return { _current, _current+end };
		}

		optional<Enumerator> TryCreateNewEnumeratorByRelativeRange(int32_t end) const
		{
			if (_current+end < _end) {
				return GetNewEnumeratorByRelativeRange(end);
			}

			return {};
		}

		Enumerator CreateNewEnumeratorByRelativeRange(int32_t start, int32_t end) const
		{
			return { _current+start, _current+end };
		}

		optional<Enumerator> TryCreateNewEnumeratorByRelativeRange(int32_t start, int32_t end) const
		{
			if (_current+start < _start || _current+end > end) {
				return {};
			}

			return GetNewEnumeratorByRelativeRange(start, end);
		}

	private:
		Iterator _current;
		Iterator _start;
		Iterator _end;
	};

	template <typename Container>
	class Enumerator<Container, true> {
	public:
		// TODO
	private:
		shared_ptr<Container> _containerPtr;
		Iterator _current;
		Iterator _start;
		Iterator _end;
	};

	// external method to implement common part method
}
