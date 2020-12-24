#pragma once
#include <vector>
#include <string>
#include <algorithm>
#include <string_view>

namespace Cmd
{
	using ::std::min;
	using ::std::move;
	using ::std::string;
	using ::std::string_view;
	using ::std::vector;

	class StringMatcher
	{
	private:
		vector<string> _candidates;

	public:
		StringMatcher(vector<string> candidates) : _candidates(move(candidates))
		{ }

		/// 应该是有可能返回空字符串的
		vector<string> Match(string keyword)
		{
			// 先考虑只有一个最小的情况，之后才考虑有多个的情况
			int minDistance = INT_MAX;
			int idxOfMinDistance = -1;

			for (auto i = 0; auto& c : _candidates)
			{
				auto d = ComputeEditDistance(c, keyword);
				if (d < minDistance)
				{
					minDistance = d;
					idxOfMinDistance = i;
				}

				++i;
			}

			if (idxOfMinDistance == -1)
			{
				return {};
			}
			else
			{
				return { _candidates[idxOfMinDistance] };
			}
		}

	private:
		static int ComputeEditDistance(string_view s1, string_view s2)
		{
			if (s1.empty())
			{
				return s2.size();
			}
			else if (s2.empty())
			{
				return s1.size();
			}

			if (s1[0] == s2[0])
			{
				return ComputeEditDistance(s1.substr(1), s2.substr(1));
			}
			else
			{
				return min({
					ComputeEditDistance(s1, s2.substr(1)) + 1,
					ComputeEditDistance(s1.substr(1), s2) + 1,
					ComputeEditDistance(s1.substr(1), s2.substr(1)) + 1,
				});
			}
		}
	};
}


