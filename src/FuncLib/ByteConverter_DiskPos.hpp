#pragma once
#include <memory>
#include <cstddef>
#include <type_traits>
#include <memory>

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

		static array<byte, Size> ConvertToByte(ThisType const& p)
		{
			return ByteConverter<Index>::ConvertToByte(p._start);
		}

		static ThisType ConvertFromByte(shared_ptr<FileReader> reader)
		{
			auto p = ByteConverter<Index>::ConvertFromByte(reader);
			// 如何设计 DiskPos 呢？
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