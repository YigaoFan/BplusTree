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

	template <template <typename> class Getter, typename... Ts>
	struct Sum
	{
		static constexpr auto Result = (... + Getter<Ts>::Result);
	};

	using ::std::tuple;

	template <template <typename> class Getter, typename... Ts>
	struct Sum<Getter, tuple<Ts...>>
	{
		static constexpr auto Result = Sum<Getter, Ts...>::Result;
	};

	template <template <typename> class Getter, typename... Ts>
	struct Max;

	template <template <typename> class Getter, typename T, typename... Ts>
	struct Max<Getter, T, Ts...>
	{
	private:
		static constexpr auto r1 = Getter<T>::Result;
		static constexpr auto r2 = Max<Getter, Ts...>::Result;
	public:
		static constexpr auto Result = r1 < r2 ? r2 : r1;
	};

	template <template <typename> class Getter, typename T>
	struct Max<Getter, T>
	{
		static constexpr auto Result = Getter<T>::Result;
	};

	template <template <typename> class Getter, typename... Ts>
	struct Max<Getter, tuple<Ts...>>
	{
		static constexpr auto Result = Max<Getter, Ts...>::Result;
	};

	// Below code ref from:
	// https://stackoverflow.com/questions/9065081/how-do-i-get-the-argument-types-of-a-function-pointer-in-a-variadic-template-cla
	template <typename T>
	struct FuncTraits;

	using ::std::size_t;

	template <typename R, typename... Args>
	struct FuncTraits<R(Args...)>
	{
		static constexpr size_t ArgCount = sizeof...(Args);
		// typedef R result_type;
		template <size_t i>
		struct Arg
		{
			using Type = typename std::tuple_element<i, std::tuple<Args...>>::type;
		};
	};

	// 可以用继承简化这个的代码
	// 这种适用的是普通的函数，奇怪这种和上种在什么情况下会分别用到
	template <typename R, typename... Args>
	struct FuncTraits<R (*)(Args...)> : public FuncTraits<R(Args...)> 
	// 这里使用模板，是可以直接指定模板参数，还是也像外面使用一样可以使用偏特化。报错了，原来还是看依照最初声明的 108 行那块。
	{
	};

	template <typename R, typename C, typename... Args>
	struct FuncTraits<R (C::*)(Args...)> : public FuncTraits<R(Args...)>
	{
	};

	template <typename R, typename... Args>
	struct FuncTraits<R(Args...) const> : public FuncTraits<R(Args...)> // 这里好像是省略了指针那个*
	{
	};

	template <typename T>
	struct GetMemberFuncType;

	template <typename R, typename T>
	struct GetMemberFuncType<R T::*> // 这里的 R 为什么是 method 的类型呢？这里好像是省略了参数类型，但这个 get 的却是 MemberFuncType
	{
		using Result = R;
	};
}
