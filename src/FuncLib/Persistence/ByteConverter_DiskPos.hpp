#pragma once
#include <memory>

namespace FuncLib::Persistence
{
	using ::std::shared_ptr;

	template <typename T>
	struct ByteConverter<DiskPos<T>, false>
	{
		using ThisType = DiskPos<T>;
		using DataMemberType = decltype(declval<ThisType>()._label);
		static constexpr bool SizeStable = All<GetSizeStable, DataMemberType>::Result;
		static constexpr size_t Size = Sum<GetSize, DataMemberType>::Result;

		/// Write pos and pointing object
		static void WriteDown(ThisType const& p, IWriter auto* writer, shared_ptr<T> const& object)
		{
			ByteConverter<DataMemberType>::WriteDown(p._label, writer);
			auto subWriter = writer->ConstructSub(p._label);
			if (object != nullptr)
			{
				p.WriteObject(object, subWriter);
			}
		}

		/// Just write pos, such as OwnerLessDiskPtr need
		static void WriteDown(ThisType const& p, IWriter auto* writer)
		{
			ByteConverter<DataMemberType>::WriteDown(p._label, writer);
		}

		static ThisType ReadOut(IReaderWithFile auto* reader)
		{
			auto p = ByteConverter<DataMemberType>::ReadOut(reader);
			auto file = reader->GetLessOwnershipFile();
			return { file, p };
		}
	};
}