#pragma once
#include <type_traits>
#include <string>
#include <memory>
#include "../Basic/TypeTrait.hpp"
#include "../FuncLib/Persistence/FriendFuncLibDeclare.hpp"

namespace Collections
{
	using ::Basic::CompileIf;
	using FuncLib::Persistence::OwnerLessDiskRef;
	using FuncLib::Persistence::OwnerState;
	using FuncLib::Persistence::TypeConverter;
	using FuncLib::Persistence::UniqueDiskPtr;
	using ::std::is_fundamental_v;
	using ::std::reference_wrapper;
	using ::std::string;
	using ::std::unique_ptr;

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
	template <typename RawType>
	struct TypeSelector<StorePlace::Disk, Refable::Yes, RawType>
	{
		using Result = typename TypeConverter<RawType, OwnerState::OwnerLess>::To;
	};

	template <typename RawType>
	struct TypeSelector<StorePlace::Disk, Refable::No, RawType>
	{
		using Result = typename TypeConverter<RawType, OwnerState::FullOwner>::To;
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

	struct TypeConfig
	{
		template <StorePlace place>
		struct Ptr;

		template <>
		struct Ptr<StorePlace::Disk>
		{
			template <typename... Ts>
			using Type = UniqueDiskPtr<Ts...>;
		};

		template <>
		struct Ptr<StorePlace::Memory>
		{
			template <typename... Ts>
			using Type = unique_ptr<Ts...>;
		};
		};
	
}
