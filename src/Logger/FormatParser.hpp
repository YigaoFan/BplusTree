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

	template <typename List>
	struct Length
	{
		static constexpr int Result = 1 + Length<typename List::Cdr>::Result;
	};

	template <>
	struct Length<Null>
	{
		static constexpr int Result = 0;
	};

	template <typename List, int Index>
	struct Get
	{
		using Result = typename Get<typename List::Cdr, Index - 1>::Result;
	};

	template <typename List>
	struct Get<List, 0>
	{
		using Result = typename List::Car;
	};

	template <typename List, typename Item>
	struct Append
	{
		template <typename Remain>
		struct Recur
		{
			using Result = Cons<typename Remain::Car, typename Recur<typename Remain::Cdr>::Result>;
		};

		template <>
		struct Recur<Null>
		{
			using Result = Cons<Item, Null>;
		};

		using Result = typename Recur<List>::Result;
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
			using Current = typename FormatMap<SecondChar>::To;
			// how to use parser combinator here
			if constexpr (sizeof...(RemainChars) == 0)
			{
				return Cons<Current, Null>();
			}
			else
			{
				return Cons<Current, decltype(ParseFormat(integer_sequence<char, RemainChars...>()))>();
			}
		}
		else if constexpr (FirstChar == ' ')
		{
			return ParseFormat(integer_sequence<char, SecondChar, RemainChars...>());
		}

		// return Null(); // error
	}
}
