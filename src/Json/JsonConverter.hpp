#pragma once
#include <string>
#include "Json.hpp"

namespace Json
{
	using ::std::string;

	class JsonConverter
	{
	public:
		template <typename T>
		static string Serialize(T const& t)
		{
			
		}

		template <typename T>
		static T Deserialize(string const& jsonStr)
		{
			// 在 T 中实现 deserialize 的方法，或者一些相关信息的代码
			// make T class data member as tuple and iterate them
			// each item corresponding a Deserialize specialization function
		}

		template <typename T>
		static T Deserialize(JsonObject const& json)
		{

		}
	};
}
