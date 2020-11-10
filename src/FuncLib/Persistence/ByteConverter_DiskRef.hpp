#pragma once

namespace FuncLib::Persistence
{
	template <typename T>
	struct ByteConverter<OwnerLessDiskRef<T>, false>
	{
		using ThisType = OwnerLessDiskRef<T>;
		using DataMemberType = decltype(declval<ThisType>()._ptr);
		static constexpr bool SizeStable = All<GetSizeStable, DataMemberType>::Result;
		static constexpr size_t Size = SizeStable ? Sum<GetSize, DataMemberType>::Result : SIZE_MAX;

		static void WriteDown(ThisType const& t, IWriter auto* writer)
		{
			ByteConverter<DataMemberType>::WriteDown(t._ptr, writer);
		}

		static ThisType ReadOut(FileReader* reader)
		{
			return ByteConverter<DataMemberType>::ReadOut(reader);
		}
	};

	template <typename T>
	struct ByteConverter<UniqueDiskRef<T>, false>
	{
		using ThisType = UniqueDiskRef<T>;
		using DataMemberType = decltype(declval<ThisType>()._ptr);
		static constexpr bool SizeStable = All<GetSizeStable, DataMemberType>::Result;
		static constexpr size_t Size = SizeStable ? Sum<GetSize, DataMemberType>::Result : SIZE_MAX;

		static void WriteDown(ThisType const& t, IWriter auto* writer)
		{
			ByteConverter<DataMemberType>::WriteDown(t._ptr, writer);
		}

		static ThisType ReadOut(FileReader* reader)
		{
			return ByteConverter<DataMemberType>::ReadOut(reader);
		}
	};
}
