#include "JsonConverter.hpp"

namespace Json::JsonConverter
{
	template <>
	JsonObject Serialize<string>(string const& t)
	{
		return JsonObject(t);
	}

	template <>
	JsonObject Serialize(int const& t)
	{
		return JsonObject(static_cast<double>(t));
	}

	template <>
	JsonObject Serialize(double const& t)
	{
		return JsonObject(t);
	}

	template <>
	JsonObject Serialize(bool const& t)
	{
		return JsonObject(t);
	}

	template <>
	JsonObject Serialize(JsonObject const& t)
	{
		return t;
	}

	template <>
	string Deserialize(JsonObject const& json)
	{
		return json.GetString();
	}

	template <>
	int Deserialize(JsonObject const& json)
	{
		return static_cast<int>(json.GetNumber());
	}

	template <>
	double Deserialize(JsonObject const& json)
	{
		return json.GetNumber();
	}

	template <>
	bool Deserialize(JsonObject const& json)
	{
		return json.GetBool();
	}
}
