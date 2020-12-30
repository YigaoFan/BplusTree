#pragma once
#include "../Basic/TypeTrait.hpp"

namespace Log
{
	using Basic::ClassOf_T;
	using Basic::FuncTraits;
	using ::std::add_pointer_t;
	using ::std::forward;
	using ::std::is_member_function_pointer_v;
	using ::std::is_reference_v;
	using ::std::move;

	// Object is pointer
	template <typename Func, typename Object, typename... Args>
	auto CallObjectMethod(Func func, Object object, Args... args)
	{
		// 可以使用 std::mem_fn 将成员方法转换为第一个参数为对象指针的函数
		return (object->*func)(forward<Args>(args)...);
	}

	template <int CurrentArgIdx, bool IsObjectMethod, typename FuncInfo, typename Func, typename... PreviousArgs>
	auto DoCurry(Func func, PreviousArgs&&... previousArgs)
	{
			constexpr int objectPtrIdx = -1;
			
			if constexpr (CurrentArgIdx == FuncInfo::ArgCount)
			{
				if constexpr (IsObjectMethod)
				{
					return CallObjectMethod(move(func), forward<PreviousArgs>(previousArgs)...);
				}
				else
				{
					return func(forward<PreviousArgs>(previousArgs)...);
				}
			}
			else if constexpr (IsObjectMethod and CurrentArgIdx == objectPtrIdx)
			{
				using ObjectPtr = add_pointer_t<ClassOf_T<Func>>;
				return [func = move(func), ... previousArgs = forward<PreviousArgs>(previousArgs)](ObjectPtr obj) mutable
				{
					return DoCurry<0, IsObjectMethod, FuncInfo>(move(func), forward<PreviousArgs>(previousArgs)..., forward<ObjectPtr>(obj));
				};
			}
			else if constexpr (CurrentArgIdx < FuncInfo::ArgCount)
			{
				using Arg = typename FuncInfo::template Arg<CurrentArgIdx>::Type;
				return [func = move(func), ... previousArgs = forward<PreviousArgs>(previousArgs)](Arg arg) mutable
				{
					return DoCurry<CurrentArgIdx + 1, IsObjectMethod, FuncInfo>(move(func), forward<PreviousArgs>(previousArgs)..., forward<Arg>(arg));
				};
			}
	}

	template <typename Func>
	auto Curry(Func func)
	{
		constexpr bool isObjMethod = is_member_function_pointer_v<Func>;
		return DoCurry<isObjMethod ? -1 : 0, isObjMethod, FuncTraits<Func>>(func);
	}
}
