#pragma once

namespace FuncLib
{
	template <typename T>
	struct ByteConverter<DiskRef<T>, false>
	{
		using ThisType = DiskRef<T>;

		static void WriteDown(ThisType const& t, shared_ptr<FileWriter> writer)
		{
			ByteConverter<decltype(t._ptr)>::WriteDown(t._ptr, writer);
		}

		static ThisType ReadOut(shared_ptr<FileReader> reader)
		{
			return ByteConverter<decltype(declval<ThisType>()._ptr)>::ReadOut(reader);
		}
	};
}
