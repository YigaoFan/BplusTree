#pragma once
#include <type_traits>

namespace Basic
{
	template<typename Test, template<typename...> class Ref>
	struct IsSpecialization : std::false_type {};

	template<template<typename...> class Ref, typename... Args>
	struct IsSpecialization<Ref<Args...>, Ref> : std::true_type {};
}
