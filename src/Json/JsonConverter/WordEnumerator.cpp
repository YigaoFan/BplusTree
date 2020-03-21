#include "WordEnumerator.hpp"
#include "../../Basic/Exception.hpp"

namespace Json::JsonConverter
{
	using ::std::size_t;
	using ::std::string_view;
	using ::std::vector;
	using ::std::move;
	using ::Collections::IEnumerator;
	using ::Basic::InvalidAccessException;

	// 假设你要求的词是不跨行的，比如一个词分布相邻的两个 vector 项中，这是不允许的
	WordEnumerator::WordEnumerator(vector<string_view> strs, char separator)
		: _strs(move(strs)), _separator(separator)
	{ }

	void WordEnumerator::ChangeSeparator(char newSeparator)
	{
		_separator = newSeparator;
	}

	string_view WordEnumerator::Current()
	{
		if (_firstMove)
		{
			throw InvalidAccessException();
		}

		return _current;
	}

	// 要考虑这几个函数被重复调用的情况
	bool WordEnumerator::MoveNext()
	{
		if (_strs.size() == 0)
		{
			return false;
		}

		if (_firstMove)
		{
			_firstMove = false;
		}

		for (; !MoveInStrView(); ++_i, _j = 0)
		{
			if (_i == _strs.size() - 1)
			{
				return false;
			}
		}

		_current = GetCurrent();
		++_count;
		return true;
	}

	size_t WordEnumerator::CurrentIndex()
	{
		using ::Basic::AccessOutOfRangeException;

		if (_count == 0)
		{
			throw AccessOutOfRangeException();
		}

		return _count - 1;
	}

	bool WordEnumerator::MoveInStrView()
	{
		for (; _j < _strs[_i].size(); ++_j)
		{
			auto c = _strs[_i][_j];
			if (c != _separator)
			{
				return true;
			}
		}

		return false;
	}

	string_view WordEnumerator::GetCurrent()
	{
		// First is checked in MoveNext()
		auto start = _j;
		_j = _j + 1;
		for (decltype(_j) count = 1; _j < _strs[_i].size(); ++_j, ++count)
		{
			auto c = _strs[_i][_j];
			if (c == _separator)
			{
				return _strs[_i].substr(start, count);
			}
		}

		++_j;
		return _strs[_i].substr(start);
	}
}
