#pragma once
#include <string>
#include <variant>
#include <vector>
#include <map>
#include <memory>

namespace Json {
	using std::string;
	using std::variant;
	using std::vector;
	using std::to_string;
	using std::map;
	using std::shared_ptr; // In Json semantic, there are some data will be shared with external, so shared_ptr

	struct Object {};
	struct Array  {};
	struct Number {};
	struct String {};
	struct True   {};
	struct False  {};
	struct Null   {};

	// how to read data from Json
	class Json {
		friend class Parser;
	private:
		enum Type {
			Object,
			Array,
			Number,
			String,
			True,
			False,
			Null,
		};
		Type _type;
		using _Array = vector<shared_ptr<Json>>;
		using _Object = map<string, shared_ptr<Json>>;
		variant<string, _Array, double, _Object> _content;

	public:
		explicit Json(struct Object, map<string, shared_ptr<Json>> object)
			: _type(Type::Object), _content(std::move(object))
		{ }

		explicit Json(struct Array, vector<shared_ptr<Json>> array)
			: _type(Type::Array), _content(std::move(array))
		{ }

		explicit Json(struct Number, double num)
			: _type(Type::Number), _content(num)
		{ }

		explicit Json(struct String, string str)
			: _type(Type::String), _content(std::move(str))
		{ }

		explicit Json(struct True)
			: _type(Type::True)
		{ }

		explicit Json(struct False)
			: _type(Type::False)
		{ }

		explicit Json(struct Null)
			: _type(Type::Null)
		{ }

		// Json(const Json& that) deep copy?
		Json(Json&& that) noexcept
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
			return *(std::get<_Object>(_content)[key]);
		}

		Json&
		operator[] (const Json& key)
		{
			assert(isObject() && key.isString());
			return operator[](key.getString());
		}

		Json&
		operator[] (size_t i)
		{
			assert(isArray());
			return *(std::get<_Array>(_content)[i]);
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

		const _Object&
		getObject() const
		{
			assert(isObject());
			return std::get<_Object>(_content);
		}
		// how modify array item external
		const _Array&
		getArray() const
		{
			assert(isArray());
			return std::get<_Array>(_content);
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
			return std::get<string>(_content);
		}

		bool
		getBool() const
		{
			return _type == Type::True;
		}

		// TODO need getNull()?
		// string
		// toString()
		// {
		// 	switch (_type) {
		// 		case Type::Object:
		// 			string str = "{";
		// 			auto objectMap = std::get<map<string, Json*>>(_content);
		// 			for (auto& pair : objectMap) {
		// 				str += pair.first;
		// 				str += ':';
		// 				str += pair.second->toString();
		// 				// need to replace += with string modify method
		// 			}
		// 			return str;
		//
		// 		case Type::Array:
		// 			// TODO
		// 			break;
		//
		// 		case Type::Number:
		// 			return to_string(std::get<double>(_content));
		//
		// 		case Type::String:
		// 			return std::get<string>(_content);
		//
		// 		case Type::True:
		// 			return "true";
		//
		// 		case Type::False:
		// 			return "false";
		//
		// 		case Type::Null:
		// 			return "null";
		// 	}
		// }
	};
}