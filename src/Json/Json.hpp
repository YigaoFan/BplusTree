#pragma once
#include <string>
#include <variant>
#include <vector>

namespace Json {
	using std::string;
	using std::variant;
	using std::vector;

	enum class Type {
		Object,
		Array,
		Number,
		String,
		True,
		False,
		Null,
	};

	class Json {
	private:
		Type _type;
		variant<string, vector<Json*>> _content;

	public:
		struct Object {};
		struct Array  {};
		struct Number {};
		struct String {};
		struct True   {};
		struct False  {};
		struct Null   {};

		// below should move to Json.cpp
		Json(Object)
			: _type(Type::Object)
		{
		}

		Json(Array)
			: _type(Type::Array)
		{
		}

		Json(Number)
			: _type(Type::Number)
		{
		}

		Json(String)
			: _type(Type::String)
		{
		}

		Json(True)
			: _type(Type::True)
		{
		}

		Json(False)
			: _type(Type::False)
		{
		}

		Json(Null)
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