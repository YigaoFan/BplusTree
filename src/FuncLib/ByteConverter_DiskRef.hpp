#pragma once

namespace FuncLib
{
	template <typename T>
	struct ByteConverter<DiskRef<T>, false>
	{
		using ThisType = DiskRef<T>;
		static constexpr size_t Size = ByteConverter<decltype(declval<ThisType>()._ptr)>::Size;

		static array<byte, Size> ConvertToByte(ThisType const& t)
		{
			return ByteConverter<decltype(t._ptr)>::ConvertToByte(t._ptr);
		}

		static ThisType ConvertFromByte(shared_ptr<File> file, uint32_t startInFile)
		{
			return ByteConverter<decltype(declval<ThisType>()._ptr)>::ConvertFromByte(file, startInFile);
		}
	};
}
