#include <type_traits>
#include "../Basic/TypeTrait.hpp"
#include "../FuncLib/FriendFuncLibDeclare.hpp"

namespace Collections
{
	using ::Basic::CompileIf;
	using ::std::is_fundamental_v;
	using ::std::reference_wrapper;
	using FuncLib::TypeConverter;

	enum class StorePlace
	{
		Memory,
		Disk,
	};

	enum class Refable
	{
		Yes,
		No,
	};

	template <StorePlace Place, Refable RefProperty, typename RawType>
	struct TypeSelector;

	template <typename RawType, Refable RefProperty>
	struct TypeSelector<StorePlace::Disk, RefProperty, RawType>
	{
		using Result = typename TypeConverter<RawType>::To;
	};

	template <typename RawType>
	struct TypeSelector<StorePlace::Memory, Refable::No, RawType>
	{
		using Result = RawType;
	};

	template <typename RawType>
	struct TypeSelector<StorePlace::Memory, Refable::Yes, RawType>
	{
		using Result = typename CompileIf<is_fundamental_v<RawType>, RawType, reference_wrapper<RawType const>>::Type;
	};
}
