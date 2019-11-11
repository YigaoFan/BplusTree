#pragma once
/**********************************
   Enumerator in Util
***********************************/

#include <functional>

namespace Util {
	using ::std::vector;
	using ::std::ref;

	/**
	 * Enumerator is just like iterator, will not copy
	 */
	template <typename Container>
	class Enumerator {
	private:
		using T = Container::value_type; // should use a type trait to support some type like raw array
		using Iterator = Container::iterator;
	public:
		ref<T> Current;

		// should be one of items: vector, array, list, raw array?
		// static_assert()
		Enumerator(Container<T, Alloc>& container)
			: _current(container.begin()), _end(container.end())
		{ }

		void sort()
		{

		}

		bool next() const
		{
			if (_current != _end) {
				Current = ref(*++_current);
				return true;
			}

			return false;
		}

	private:
		Iterator _current;
		Iterator _end;
	};
}
