#include "../Basic/TypeTrait.hpp"

namespace Server
{
	using Basic::FuncTraits;
	using ::std::forward;
	using ::std::is_member_function_pointer_v;
	using ::std::is_reference_v;
	using ::std::move;

	// Object is pointer
	template <typename Func, typename Object, typename... Args>
	auto CallObjectMethod(Func func, Object object, Args... args)
	{
		return (object->*func)(forward<Args>(args)...);
	}

	template <size_t CurrentArgIdx, bool IsObjectMethod, typename FuncInfo, typename Func, typename... PreviousArgs>
	auto DoCurry(Func func, PreviousArgs&&... previousArgs)
	{
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
		else if constexpr (CurrentArgIdx < FuncInfo::ArgCount)
		{
			return [move(func), ... previousArgs = forward<PreviousArgs>(previousArgs)]<typename Arg>(Arg&& arg) mutable
			{
				return DoCurry<CurrentArgIdx + 1, IsObjectMethod, FuncInfo>(move(func), forward<PreviousArgs>(previousArgs)..., forward<Arg>(arg));
			};
		}
	}

	template <typename Func>
	auto Curry(Func func)
	{
		constexpr bool isObjMethod = is_member_function_pointer_v<Func>;
		return DoCurry<0, isObjMethod, FuncTraits<Func>>(func);
	}
}
