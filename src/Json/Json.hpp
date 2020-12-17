#pragma once
#include <string>
#include <variant>
#include <vector>
#include <map>

namespace Json
{
	using ::std::string;
	using ::std::size_t;
	using ::std::variant;
	using ::std::vector;
	using ::std::to_string;
	using ::std::map;
	using ::std::move;
	using ::std::get;

	enum JsonType
	{
		Object,
		Array,
		Number,
		String,
		True,
		False,
		Null,
	};

	class JsonObject 
	{
	public:
		using _Array = vector<JsonObject>;
		using _Object = map<string, JsonObject>;
	private:
		friend class Parser;
		JsonType _type;
		variant<string, _Array, double, _Object> _content;
	public:
		JsonObject();
		explicit JsonObject(_Object object);
		explicit JsonObject(_Array array);
		explicit JsonObject(double num);
		explicit JsonObject(string str);
		explicit JsonObject(bool value);
		JsonObject(JsonObject&& that) noexcept;
		JsonObject(JsonObject const& that);

		bool IsObject() const;
		bool IsArray () const;
		bool IsNumber() const;
		bool IsString() const;
		bool IsTrue  () const;
		bool IsFalse () const;
		bool IsBool  () const;
		bool IsNull  () const;

		JsonObject& operator[] (string const& key);
		JsonObject& operator[] (JsonObject const& key);
		JsonObject& operator[] (size_t i);
		JsonObject const& operator[] (string const& key)     const;
		JsonObject const& operator[] (JsonObject const& key) const;
		JsonObject const& operator[] (size_t i)              const;

		JsonObject& operator= (JsonObject const& that);
		JsonObject& operator= (JsonObject&& that);
		JsonObject& operator= (_Array array);
		JsonObject& operator= (_Object obj);
		JsonObject& operator= (double num);
		JsonObject& operator= (string str);
		JsonObject& operator= (bool value);
		JsonObject& operator= (decltype(nullptr) null);

		// Think necessary
		_Object const& GetObject() const;
		// // how modify array item external
		_Array const& GetArray()   const;
		double GetNumber()         const;
		string GetString()         const;
		bool   GetBool()           const;
		size_t Count()             const;

		string ToString()          const;
	};
}