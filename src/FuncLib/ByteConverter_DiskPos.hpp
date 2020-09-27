#pragma once
#include <memory>

namespace FuncLib
{
	using ::std::shared_ptr;

	template <typename T>
	struct ByteConverter<DiskPos<T>, false>
	{
		using ThisType = DiskPos<T>;
		using DataMemberType = decltype(declval<ThisType>()._lable);

		static constexpr bool SizeStable = All<GetSizeStable, DataMemberType>::Result;

		/// only write down DiskPos self, not write the object corresponding to the position
		static void WriteDown(ThisType const& p, shared_ptr<FileWriter> writer)
		{
			ByteConverter<DataMemberType>::WriteDown(p._lable, writer);
		}

		static void WriteDown(ThisType const& p, shared_ptr<FileWriter> writer, shared_ptr<T> const& object)
		{
			WriteDown(p, writer);
			if (object != nullptr)
			{
				p.WriteObject(object);
			}
		}

		static ThisType ReadOut(shared_ptr<FileReader> reader)
		{
			auto p = ByteConverter<DataMemberType>::ReadOut(reader);
			auto file = reader->GetLessOwnershipFile();
			return { file, p };
		}
	};
}