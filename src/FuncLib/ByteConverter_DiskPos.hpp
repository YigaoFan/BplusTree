#pragma once
#include <memory>

namespace FuncLib
{
	using ::std::shared_ptr;

	template <typename T>
	struct ByteConverter<DiskPos<T>, false>
	{
		using ThisType = DiskPos<T>;
		using DataMemberType = decltype(declval<ThisType>().Addr());

		static constexpr bool SizeStable = All<GetSizeStable, DataMemberType>::Result;

		static void WriteDown(ThisType const& p, shared_ptr<FileWriter> writer)
		{
			ByteConverter<DataMemberType>::WriteDown(p._start, writer);
		}

		static ThisType ReadOut(shared_ptr<FileReader> reader)
		{
			auto p = ByteConverter<DataMemberType>::ReadOut(reader);
			auto file = reader->GetLessOwnershipFile();
			return { file, p };
		}
	};

	namespace Store
	{
		template <typename T, typename... Ts>
		shared_ptr<InsidePositionOwner> MakePositionOwner(Ts... ts)
		{
			using ::std::make_shared;
			return make_shared<DiskPos<T>>(ts...);
		}
	}
}