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

		/// only write down DiskPos self, not write the object corresponding to the position
		static void WriteDown(ThisType const& p, IWriter auto* writer)
		{
			ByteConverter<DataMemberType>::WriteDown(p._label, writer);
		}

		static void WriteDown(ThisType const& p, IWriter auto* writer, shared_ptr<T> const& object)
		{
			WriteDown(p, writer);
			if (object != nullptr)
			{
				p.WriteObject(object, writer);
			}
		}

		static ThisType ReadOut(FileReader* reader)
		{
			auto p = ByteConverter<DataMemberType>::ReadOut(reader);
			auto file = reader->GetLessOwnershipFile();
			return { file, p };
		}
	};
}