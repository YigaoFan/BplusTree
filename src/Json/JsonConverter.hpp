#pragma once
#include <string>

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
		static T Deserialize(string jsonStr)
		{

		}
    };
}
