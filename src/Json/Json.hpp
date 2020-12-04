#pragma once
#include <string>
#include <variant>
#include <vector>
#include <map>
#include <memory>

namespace Json
{
	using ::std::string;
	using ::std::size_t;
	using ::std::variant;
	using ::std::vector;
	using ::std::to_string;
	using ::std::map;
	using ::std::move;
	using ::std::shared_ptr; // In Json semantic, there are some data will be shared with external, so shared_ptr
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
	// how to read data from Json
	class JsonObject 
	{
		friend class Parser;
	private:
		JsonType _type;
		using _Array = vector<shared_ptr<JsonObject>>; // maybe need to be public?
		using _Object = map<string, shared_ptr<JsonObject>>;
		variant<string, _Array, double, _Object> _content;
	public:
		// How to dynamic cons JsonObject with any type? has this demand?
		JsonObject();
		explicit JsonObject(_Object object);
		explicit JsonObject(vector<shared_ptr<JsonObject>> array);
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
		// TODO test if previous has shared_ptr, will it be deconstructed when assign?
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

		string ToString();
	};
}