#pragma once

namespace FuncLib
{
	template <typename T>
	struct ByteConverter<DiskRef<T>, false>
	{
		using ThisType = DiskRef<T>;

		static void ConvertToByte(ThisType const& t, shared_ptr<FileWriter> writer)
		{
			ByteConverter<decltype(t._ptr)>::ConvertToByte(t._ptr, writer);
		}

		static ThisType ConvertFromByte(shared_ptr<FileReader> reader)
		{
			return ByteConverter<decltype(declval<ThisType>()._ptr)>::ConvertFromByte(reader);
		}
	};
}
