#include "Json.hpp"
#include "../Basic/Exception.hpp"
#include "../Basic/Debug.hpp"
#include "ParseException.hpp"

namespace Json
{
	using ::Basic::Assert;
	using ::Basic::InvalidAccessException;
	using ::std::get;
	using ::std::map;
	using ::std::move;
	using ::std::shared_ptr; // In Json semantic, there are some data will be shared with external, so shared_ptr
	using ::std::string;
	using ::std::to_string;
	using ::std::variant;
	using ::std::vector;

	// How to dynamic cons JsonObject with any type? has this demand?
	JsonObject::JsonObject() : _type(JsonType::Null) {}
	JsonObject::JsonObject(_Object object) : _type(JsonType::Object), _content(move(object)) {}
	JsonObject::JsonObject(_Array array) : _type(JsonType::Array), _content(move(array)) {}
	JsonObject::JsonObject(double num) : _type(JsonType::Number), _content(num) {}
	JsonObject::JsonObject(string str) : _type(JsonType::String), _content(move(str)) {}
	JsonObject::JsonObject(bool value) : _type(value ? JsonType::True : JsonType::False) {}
	JsonObject::JsonObject(JsonObject&& that) noexcept : _type(that._type), _content(move(that._content)) {}
	JsonObject::JsonObject(JsonObject const& that) : _type(that._type), _content(that._content) {}

	bool JsonObject::IsObject() const { return _type == JsonType::Object; }
	bool JsonObject::IsArray() const { return _type == JsonType::Array; }
	bool JsonObject::IsNumber() const { return _type == JsonType::Number; }
	bool JsonObject::IsString() const { return _type == JsonType::String; }
	bool JsonObject::IsTrue() const { return _type == JsonType::True; }
	bool JsonObject::IsFalse() const { return _type == JsonType::False; }
	bool JsonObject::IsBool() const { return IsTrue() || IsFalse(); }
	bool JsonObject::IsNull() const { return _type == JsonType::Null; }

	JsonObject& JsonObject::operator[] (string const& key) { Assert(IsObject()); return get<_Object>(_content).at(key); }
	JsonObject& JsonObject::operator[] (JsonObject const& key) { Assert(IsObject() && key.IsString()); return operator[](key.GetString()); }
	JsonObject& JsonObject::operator[] (size_t i) { Assert(IsArray()); return get<_Array>(_content)[i]; }

	JsonObject const& JsonObject::operator[] (string const& key)     const { return const_cast<JsonObject*>(this)->operator[](key); }
	JsonObject const& JsonObject::operator[] (JsonObject const& key) const { return const_cast<JsonObject*>(this)->operator[](key); }
	JsonObject const& JsonObject::operator[] (size_t i)              const { return const_cast<JsonObject*>(this)->operator[](i); }

	JsonObject& JsonObject::operator= (JsonObject const& that)
	{
		this->_type = that._type;
		this->_content = that._content;
		return *this;
	}

	JsonObject& JsonObject::operator= (JsonObject&& that)
	{
		this->_type = that._type;
		this->_content = move(that._content);
		that._type = JsonType::Null;
		return *this;
	}

	// TODO test if previous has shared_ptr, will it be deconstructed when assign?
	JsonObject& JsonObject::operator= (_Array array)
	{
		this->_type = JsonType::Array;
		this->_content = move(array);
		return *this;
	}

	JsonObject& JsonObject::operator= (_Object obj)
	{
		this->_type = JsonType::Object;
		this->_content = move(obj);
		return *this;
	}

	JsonObject& JsonObject::operator= (double num)
	{
		this->_type = JsonType::Number;
		this->_content = num;
		return *this;
	}

	JsonObject& JsonObject::operator= (string str)
	{
		this->_type = JsonType::String;
		this->_content = move(str);
		return *this;
	}

	JsonObject& JsonObject::operator= (bool value)
	{
		this->_type = value ? JsonType::True : JsonType::False;
		return *this;
	}

	JsonObject& JsonObject::operator= (decltype(nullptr) null)
	{
		this->_type = JsonType::Null;
		return *this;
		/* need to release previous resource?*/
	}

	// Think necessary
	JsonObject::_Object const& JsonObject::GetObject() const
	{
		Assert(IsObject());
		return get<_Object>(_content);
	}

	// // how modify array item external
	JsonObject::_Array const& JsonObject::GetArray() const
	{
		Assert(IsArray());
		return get<_Array>(_content);
	}

	double JsonObject::GetNumber() const
	{
		Assert(IsNumber());
		return get<double>(_content);
	}

	string JsonObject::GetString() const
	{
		Assert(IsString());
		return get<string>(_content);
	}

	bool JsonObject::GetBool() const
	{
		Assert(IsBool());
		return _type == JsonType::True;
	}

	size_t JsonObject::Count() const
	{
		switch (_type)
		{
		case Json::Object:
			return GetObject().size();
		case Json::Array:
			return GetArray().size();
		default:
			throw InvalidAccessException("Only Object and Array can use JsonObject::Count()");
		}
	}

	string JsonObject::ToString() const
	{
		switch (_type)
		{
		case JsonType::Object:
		{
			string objStr{ "{" };
			auto& objectMap = GetObject(); // TODO reduce the judge in GetObject()
			for (auto& p : objectMap)
			{
				objStr += '"';
				objStr += p.first;
				objStr += '"';
				objStr += ':';
				objStr += p.second.ToString();
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
				arrStr += e.ToString();
				arrStr += ',';
			}
			arrStr += ']';
			return arrStr;
		}

		case JsonType::Number:return to_string(GetNumber());
		case JsonType::String:return string("\"") + GetString() + string("\"");
		case JsonType::True:  return "true";
		case JsonType::False: return "false";
		case JsonType::Null:  return "null";
		default: throw ProgramError("Not handled JsonType in JsonObject::ToString method");
		}
	}
}
