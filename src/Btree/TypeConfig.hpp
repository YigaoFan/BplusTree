#include <type_traits>
#include <string>
#include "../Basic/TypeTrait.hpp"
#include "../FuncLib/FriendFuncLibDeclare.hpp"

namespace Collections
{
	using ::Basic::CompileIf;
	using FuncLib::OwnerLessDiskRef;
	using FuncLib::OwnerState;
	using FuncLib::TypeConverter;
	using ::std::is_fundamental_v;
	using ::std::reference_wrapper;
	using ::std::string;

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

	/// Refable 在 disk 里的另一层意思是拥不拥有 Ownership
	template <typename RawType, Refable RefProperty>
	struct TypeSelector<StorePlace::Disk, RefProperty, RawType>
	{
		using Result = 
			typename TypeConverter<RawType, 
								   RefProperty == Refable::Yes ? OwnerState::OwnerLess : OwnerState::FullOwner
									>::To;
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
