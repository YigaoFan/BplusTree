#pragma once
#include <string>
#include <variant>
#include <vector>
#include <map>

namespace Json {
	using std::string;
	using std::variant;
	using std::vector;
	using std::to_string;
	using std::map;

	struct Object {};
	struct Array  {};
	struct Number {};
	struct String {};
	struct True   {};
	struct False  {};
	struct Null   {};
	
	// how to read data from Json
	class Json {
	private:
		enum class Type {
			Object,
			Array,
			Number,
			String,
			True,
			False,
			Null,
		};
		Type _type;
		// TODO use smart pointer
		variant<string, vector<Json*>, double, map<string, Json*>> _content;

	public:
		Json(Object, map<string, Json*> object)
			: _type(Type::Object), _content(std::move(object))
		{ }

		Json(Array, vector<Json*> array)
			: _type(Type::Array), _content(std::move(array))
		{ }

		Json(Number, double num)
			: _type(Type::Number), _content(num)
		{ }

		Json(String, string str)
			: _type(Type::String), _content(std::move(str))
		{ }

		Json(True)
			: _type(Type::True)
		{ }

		Json(False)
			: _type(Type::False)
		{ }

		Json(Null)
			: _type(Type::Null)
		{ }

		// Json(const Json& that) deep copy?
		Json(Json&& that)
			: _type(that._type), _content(std::move(that._content))
		{ }

		inline
		bool
		isObject() const
		{
			return _type == Type::Object;
		}

		inline
		bool
		isArray() const
		{
			return _type == Type::Array;
		}

		inline
		bool
		isNumber() const
		{
			return _type == Type::Number;
		}

		inline
		bool
		isString() const
		{
			return _type == Type::String;
		}

		inline
		bool
		isTrue() const
		{
			return _type == Type::True;
		}

		inline
		bool
		isFalse() const
		{
			return _type == Type::False;
		}

		inline
		bool
		isBool() const
		{
			return isTrue() || isFalse();
		}

		inline
		bool
		isNull() const
		{
			return _type == Type::Null;
		}

		Json&
		operator[] (const string& key)
		{
			assert(isObject()); // use self define exception?
			return *(std::get<map<string, Json*>>(_content)[key]);
		}

		Json&
		operator[] (const Json& key)
		{
			assert(isObject() && key.isString());
			return *(std::get<map<string, Json*>>(_content)[key]);
		}

		Json&
		operator[] (size_t i)
		{
			assert(isArray());
			return *(std::get<vector<Json*>>(_content)[i]);
		}

		// Json&
		// operator= (vector<Json*> jsonArray)
		// {
		//
		// }
		//
		// Json&
		// operator= (double num)
		// {
		//
		// }

		const map<string, Json*>&
		getObject() const
		{
			assert(isObject());
			return std::get<map<string, Json*>>(_content);
		}
		// how modify array item external
		const vector<Json*>&
		getArray() const
		{
			assert(isArray());
			return std::get<vector<Json*>>(_content);
		}

		double
		getNum() const
		{
			assert(isNumber());
			return std::get<double>(_content);
		}

		const string&
		getString() const
		{
			assert(isString());
			reutrn std::get<string>(_content);
		}

		bool
		getBool() const
		{
			return _type == Type::True;
		}

		// TODO need getNull()?
		string
		toString()
		{
			switch (_type) {
				case Type::Object:
					string str = "{";
					auto objectMap = std::get<map<string, Json*>>(_content);
					for (auto& pair : objectMap) {
						str += pair.first;
						str += ':';
						str += pair.value->toString();
						// need to replace += with string modify method
					}
					return str;

				case Type::Array:
					// TODO
					break;

				case Type::Number:
					return to_string(std::get<double>(_content));

				case Type::String:
					return std::get<string>(_content);

				case Type::True:
					return "true";

				case Type::False:
					return "false";

				case Type::Null:
					return "null";
			}
		}
	};
}