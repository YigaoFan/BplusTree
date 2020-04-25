#pragma once
#include <memory>
#include <cstddef>
#include <type_traits>

namespace FuncLib
{
	using ::std::byte;
	using ::std::declval;
	using ::std::shared_ptr;

	// 这里统一一下用的整数类型

	template <typename T>
	struct ByteConverter<DiskPos<T>, false>
	{
		using ThisType = DiskPos<T>;
		using Index = typename ThisType::Index;
		static constexpr size_t Size = ByteConverter<decltype(declval<ThisType>()._start)>::Size;

		array<byte, Size> ConvertToByte(ThisType const& p)
		{
			return ByteConverter<Index>::ConvertToByte(p._start);
		}

		ThisType ConvertFromByte(shared_ptr<File> file, uint32_t startInFile)
		{
			auto i = ByteConverter<Index>::ConvertFromByte(file, startInFile);
			return ThisType(i);
		}
	};
}