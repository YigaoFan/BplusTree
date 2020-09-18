#pragma once
#include <memory>

namespace FuncLib
{
	using ::std::shared_ptr;

	// 这里统一一下用的整数类型

	template <typename T>
	struct ByteConverter<DiskPos<T>, false>
	{
		using ThisType = DiskPos<T>;
		using DataMemberType = typename ThisType::Index;

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
		template <typename T>
		shared_ptr<IInsidePositionOwner> MakePositionOwner(shared_ptr<File> file, pos_int pos)
		{
			using ::std::make_shared;
			return make_shared<DiskPos<T>>(file, pos);
		}
	}
}