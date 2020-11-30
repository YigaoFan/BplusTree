#pragma once
/***********************************************************************************************************
   WordEnumerator in Json::JsonConverter
***********************************************************************************************************/

#include <string_view>
#include <vector>
#include "../../Btree/IEnumerator.hpp"

namespace Json::JsonConverter
{
	using ::std::size_t;
	using ::std::string_view;
	using ::std::vector;
	using ::Collections::IEnumerator;

	// 假设你要求的词是不跨行的，比如一个词分布相邻的两个 vector 项中，这是不允许的
	class WordEnumerator
	{
	private:
		bool _firstMove = true;
		string_view _current = "";
		vector<string_view> _strs;
		char _separator;
		uint32_t _i = 0;
		uint32_t _j = 0;
		uint32_t _count = 0;

	public:
		WordEnumerator(vector<string_view> strs, char separator);
		void ChangeSeparator(char newSeparator);
		string_view Current();
		bool MoveNext();
		size_t CurrentIndex();

	private:
		bool MoveInStrView();
		string_view GetCurrent();
	};
}
