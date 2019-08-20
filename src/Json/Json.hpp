#pragma once
#include <string>

namespace Json {
	using std::string;

	enum Type {
		Object,
		Array,
		Number,
		String,
		Bool,
		// Null?
	};

	class Json {
		Type type;
	public:
		// below should move to Json.cpp
		Json(/* Json element maybe */)
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

		}
	};
}