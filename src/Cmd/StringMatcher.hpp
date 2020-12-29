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

		// 为什么 Re 和 AddFunc 相近？，因为字符长度？如果是的话，要改下算法
		// 还有这里也要测试

		/// 应该是有可能返回空字符串的
		vector<string> Match(string_view keyword)
		{
			using ::std::toupper;

			if (keyword.empty())
			{
				return _candidates;
			}

			vector<string> matcheds;

			for (auto& c : _candidates)
			{
				if (c.starts_with(keyword))
				{
					matcheds.push_back(string(c));
				}
				else
				{
					auto upperVerKeyword = string(keyword);
					upperVerKeyword[0] = toupper(upperVerKeyword[0]);
					if (c.starts_with(upperVerKeyword))
					{
						matcheds.push_back(string(c));
					}
				}
			}

			return matcheds;

			// edit distance below
			// 选取多个的情况 TODO
			// 开头应该占有相当大的比重
			// int minDistance = INT_MAX;
			// int idxOfMinDistance = -1;

			// for (auto i = 0; auto& c : _candidates)
			// {
			// 	auto d = ComputeEditDistance(c, keyword);
			// 	if (d < minDistance)
			// 	{
			// 		minDistance = d;
			// 		idxOfMinDistance = i;
			// 	}

			// 	++i;
			// }

			// if (idxOfMinDistance == -1)
			// {
			// 	return {};
			// }
			// else
			// {
			// 	return { _candidates[idxOfMinDistance] };
			// }
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


