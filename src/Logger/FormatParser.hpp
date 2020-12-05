#include <algorithm>
#include <string_view>
#include "../Basic/TypeTrait.hpp"
#include "FormatMap.hpp"

namespace Log
{
	using ::std::string_view;

	template <size_t N>
	struct StringLiteral
	{
		constexpr StringLiteral(const char (&str)[N])
		{
			std::copy_n(str, N, value);
		}

		char value[N];
	};

	struct Null { };

	template <typename T1, typename T2>
	struct Cons
	{
		using Car = T1;
		using Cdr = T2;
	};

	template <typename List, typename Item>
	struct Append
	{
		template <typename Remain>
		struct Recur
		{
			using Result = Cons<Remain::Car, Recur<Remain::Cdr>::Result>;
		};

		template <>
		struct Recur<Null>
		{
			using Result = Cons<Item, Null>;
		};

		using Result = Recur<List>::Result;
	};
	
	using ::std::index_sequence;
	using ::std::integer_sequence;
	using ::std::make_index_sequence;

	template <typename LiteralGetter, size_t... Idxs>
	constexpr decltype(auto) StrToTypeImpl(LiteralGetter, index_sequence<Idxs...>)
	{
		return integer_sequence<char, LiteralGetter::Get()[Idxs]...>();
	}

	template <typename LiteralGetter>
	constexpr decltype(auto) StrToType(LiteralGetter t)
	{
		return StrToTypeImpl(t, make_index_sequence<sizeof(LiteralGetter::Get()) - 1>());
	}

#define STRING(LITERAL)                                               \
	(Log::StrToType([]                                                \
	{                                                                 \
		struct tmp                                                    \
		{                                                             \
			static constexpr decltype(auto) Get() { return LITERAL; } \
		};                                                            \
		return tmp{};                                                 \
	}()))

	template <char FirstChar, char SecondChar, char... RemainChars>
	constexpr auto ParseFormat(integer_sequence<char, FirstChar, SecondChar, RemainChars...>)
	{
		if constexpr (FirstChar == '%')
		{
			return Cons<FormatMap<SecondChar>::To, decltype(ParseFormat<RemainChars...>())>();
		}
		else if constexpr (FirstChar == ' ')
		{
			return ParseFormat<SecondChar, RemainChars...>();
		}
		// other char wait to parse

		return Null();
	}
}
