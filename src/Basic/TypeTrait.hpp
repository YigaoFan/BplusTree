#pragma once
#include <type_traits>
#include <tuple>

namespace Basic
{
	template<typename Test, template<typename...> class Ref>
	struct IsSpecialization : std::false_type {};

	template<template<typename...> class Ref, typename... Args>
	struct IsSpecialization<Ref<Args...>, Ref> : std::true_type {};

	template <bool Condition, typename A, typename B>
	struct CompileIf;

	template <typename A, typename B>
	struct CompileIf<true, A, B>
	{
		using Type = A;
	};

	template <typename A, typename B>
	struct CompileIf<false, A, B>
	{
		using Type = B;
	};

	template <typename T>
	struct ReturnType;

	template <typename R, typename... Args>
	struct ReturnType<R(Args...)>
	{
		using Type = R;
	};

	template <typename R, typename... Args>
	struct ReturnType<R(Args...) noexcept>// 原来 noexcept，const 也算类型的一部分，那之前引用这个的编译怎么通过了呢
	{
		using Type = R;
	};

	/// support unpack types in tuple
	template <template <typename> class Predicate, typename... Ts>
	struct All;

	using ::std::tuple;

	template <template <typename> class Predicate, typename... Ts>
	struct All<Predicate, tuple<Ts...>>
	{
		static constexpr bool Result = All<Predicate, Ts...>::Result;
	};

	template <template <typename> class Predicate, typename T, typename... Ts>
	struct All<Predicate, T, Ts...>
	{
		static constexpr bool Result = Predicate<T>::Result && All<Predicate, Ts...>::Result;
	};

	template <template <typename> class Predicate>
	struct All<Predicate>
	{
		static constexpr bool Result = true;
	};

	// Below code ref from:
	// https://stackoverflow.com/questions/9065081/how-do-i-get-the-argument-types-of-a-function-pointer-in-a-variadic-template-cla
	template <typename T>
	struct FuncTraits;

	using ::std::size_t;

	template <typename R, typename... Args>
	struct FuncTraits<R(Args...)>
	{
		// static const size_t nargs = sizeof...(Args);
		// typedef R result_type;
		template <size_t i>
		struct Arg
		{
			using Type = typename std::tuple_element<i, std::tuple<Args...>>::type;
		};
	};

	template <typename R, typename... Args>
	struct FuncTraits<R(Args...) const>
	{
		template <size_t i>
		struct Arg
		{
			using Type = typename std::tuple_element<i, std::tuple<Args...>>::type;
		};
	};

	template <typename T>
	struct GetMemberFuncType;

	template <typename R, typename T>
	struct GetMemberFuncType<R T::*>// 这里的 R 为什么是 method 的类型呢？
	{
		using Result = R;
	};
}
