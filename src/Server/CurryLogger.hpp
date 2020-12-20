#include "../Basic/TypeTrait.hpp"

namespace Server
{
	using Basic::FuncTraits;
	using ::std::forward;
	using ::std::is_reference_v;
	using ::std::move;

	template <size_t CurrentArgIdx, typename FuncInfo, typename Func, typename... PreviousArgs>
	auto DoCurry(Func func, PreviousArgs&&... previousArgs)
	{
		if constexpr (CurrentArgIdx == FuncInfo::ArgCount)
		{
			return func(forward<PreviousArgs>(previousArgs)...);
		}
		else if constexpr (CurrentArgIdx < FuncInfo::ArgCount)
		{
			return [move(func), ... previousArgs = forward<PreviousArgs>(previousArgs)]<typename Arg>(Arg&& arg) mutable
			{
				return DoCurry<CurrentArgIdx + 1, FuncInfo>(move(func), forward<PreviousArgs>(previousArgs)..., forward<Arg>(arg));
			};
		}
	}

	template <typename Func>
	auto Curry(Func func)
	{
		return DoCurry<0, FuncTraits<Func>>(func);
	}
}
