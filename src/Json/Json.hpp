#pragma once
#include <string>
#include <variant>
#include <vector>

namespace Json {
	using std::string;
	using std::variant;
	using std::vector;

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
		variant<string, vector<Json*>, double> _content;

	public:
		// below should move to Json.cpp
		template <typename T>
		Json
		makeJson()
		{

		}

		template <>
		Json
		makeJson<String>()
		{

		}

		template <typename T>
		Json()
		{
			static_assert(fasle, "Not support self-defined type");
		}

		template
		Json<Object>()
			: _type(Type::Object)
		{
		}

		template
		Json<Array>()
			: _type(Type::Array)
		{
		}

		template
		Json<Number>()
			: _type(Type::Number)
		{
		}

		template
		Json<String>(string str)
			: _type(Type::String), _content(std::move(str))
		{
		}

		template
		Json<True>()
			: _type(Type::True)
		{
		}

		template
		Json<False>
			: _type(Type::False)
		{
		}

		template
		Json<Null>()
			: _type(Type::Null)
		{
		}

		// 这个主要看解析的时候的需求，不用复杂。后期完了可以自己丰富
		bool
		append(Json json)
		{

		}

		string
		toString()
		{
			switch (_type) {
			case Type::Object:
				break;

			case Type::Array:
				break;

			case Type::Number:
				break;

			case Type::String:
				break;

			case Type::True:
				break;

			case Type::False:
				break;

			case Type::Null:
				break;
			}
		}
	};
}