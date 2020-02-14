#pragma once
#include <string>
#include <string_view>
#include <variant>
#include <vector>
#include <map>
#include <memory>

namespace Json
{
	using ::std::string;
	using ::std::variant;
	using ::std::vector;
	using ::std::to_string;
	using ::std::map;
	using ::std::move;
	using ::std::shared_ptr; // In Json semantic, there are some data will be shared with external, so shared_ptr
	using ::std::string_view;
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

		static void Assert(bool e, string_view message = "")
		{
			// TODO
			// if (!e) { throw InvalidAccessException(message); }
		}

	public:
		// How to dynamic cons JsonObject with any type? has this demand?
		JsonObject() : _type(JsonType::Null) { }
		explicit JsonObject(_Object object) : _type(JsonType::Object), _content(move(object)) { }
		explicit JsonObject(vector<shared_ptr<JsonObject>> array) : _type(JsonType::Array), _content(move(array)) { }
		explicit JsonObject(double num)	: _type(JsonType::Number), _content(num) { }
		explicit JsonObject(string str)	: _type(JsonType::String), _content(move(str)) { }
		explicit JsonObject(bool value) : _type(value ? JsonType::True : JsonType::False) { }
		JsonObject(JsonObject&& that) noexcept : _type(that._type), _content(move(that._content)) { }
		JsonObject(JsonObject const& that) : _type(that._type), _content(that._content) { }

		bool IsObject() const { return _type == JsonType::Object; }
		bool IsArray () const { return _type == JsonType::Array; }
		bool IsNumber() const { return _type == JsonType::Number; }
		bool IsString() const { return _type == JsonType::String; }
		bool IsTrue  () const { return _type == JsonType::True; } // Necessary?
		bool IsFalse () const { return _type == JsonType::False; }// Necessary?
		bool IsBool  () const { return IsTrue() || IsFalse(); }
		bool IsNull  () const { return _type == JsonType::Null; }

		// TODO const version
		JsonObject& operator[] (string const& key)     { Assert(IsObject()); return *(get<_Object>(_content)[key]); }
		JsonObject& operator[] (JsonObject const& key) { Assert(IsObject() && key.IsString()); return operator[](key.GetString()); }
		JsonObject& operator[] (size_t i)              { Assert(IsArray()); return *(get<_Array>(_content)[i]); }

		JsonObject& operator[] (string const& key) const     { Assert(IsObject()); return *(get<_Object>(_content)[key]); }
		JsonObject& operator[] (JsonObject const& key) const { Assert(IsObject() && key.IsString()); return operator[](key.GetString()); }
		JsonObject& operator[] (size_t i)          const     { Assert(IsArray()); return *(get<_Array>(_content)[i]); }

		JsonObject& operator= (JsonObject const& that)   { this->_type = that._type; this->_content = that._content; }
		JsonObject& operator= (JsonObject&& that)    { this->_type = that._type; this->_content = move(that._content); that._type = JsonType::Null; }
		// TODO test if previous has shared_ptr, will it be deconstructed when assign?
		JsonObject& operator= (_Array array){ this->_type = JsonType::Array; this->_content = move(array); }
		JsonObject& operator= (_Object obj) { this->_type = JsonType::Object; this->_content = move(obj); }
		JsonObject& operator= (double num)  { this->_type = JsonType::Number; this->_content = num; }
		JsonObject& operator= (string str)  { this->_type = JsonType::String; this->_content = move(str); }
		JsonObject& operator= (bool value)  { this->_type = value ? JsonType::True : JsonType::False; }
		JsonObject& operator= (decltype(nullptr) null) { this->_type = JsonType::Null; /* need to release previous resource?*/ } // Check on zhihu Moli Ye zhuan lan

		// Think necessary
		_Object const& GetObject() const { Assert(IsObject()); return get<_Object>(_content); }
		// // how modify array item external
		const _Array& GetArray()   const { Assert(IsArray()); return get<_Array>(_content); }
		double GetNumber()         const { Assert(IsNumber()); return get<double>(_content); }
		string GetString()         const { Assert(IsString()); return get<string>(_content); }
		bool   GetBool()           const { return _type == JsonType::True; }

		string ToString()
		{
			switch (_type)
			{
			case JsonType::Object:
			{
					string objStr{ "{" };
					auto& objectMap = GetObject(); // TODO reduce the judge in GetObject()
					for (auto& pair : objectMap) 
					{
						objStr += pair.first;
						objStr += ':';
						objStr += pair.second->ToString();
						objStr += ',';
					}
					objStr += '}';
					return objStr;
			}
		
			case JsonType::Array:
			{
					string arrStr{ "[" };
					auto& array = GetArray();
					for (auto& e : array) 
					{
						arrStr += e->ToString();
						arrStr += ',';
					}
					arrStr += ']';
					return arrStr;
			}
		
			case JsonType::Number:return to_string(GetNumber());
			case JsonType::String:return GetString();
			case JsonType::True:  return "true";
		 	case JsonType::False: return "false";
			case JsonType::Null:  return "null";
			}
		}
	};
}