#include <string_view>
#include <utility>

namespace Basic
{
	using ::std::pair;
	using ::std::string_view;

	string_view TrimStart(string_view s);
	string_view TrimEnd(string_view s);

	template <bool IsTrimRemainStart>
	pair<string_view, string_view> ParseOut(string_view s, string_view divider)
	{
		auto dividerPos = s.find_first_of(divider);
		auto content = s.substr(0, dividerPos);
		string_view remain;

		if (dividerPos != string_view::npos)
		{
			remain = s.substr(dividerPos + divider.size());
			if constexpr (IsTrimRemainStart)
			{
				remain = TrimStart(remain);
			}
		}

		return {content, remain};
	}
}
