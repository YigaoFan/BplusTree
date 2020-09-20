#pragma once
#include <memory>
#include "Store/File.hpp"
#include "Store/InsidePositionOwner.hpp"

namespace FuncLib
{
	using namespace Store;
	using ::std::enable_shared_from_this;
	using ::std::make_shared;
	using ::std::shared_ptr;

	template <typename T>
	class DiskPos : public InsidePositionOwner
	{
	private:
		using Base = InsidePositionOwner;
		friend struct ByteConverter<DiskPos, false>;
		template <typename>
		friend class DiskPos;

		File* _file;
	public:
		DiskPos(File* file, pos_int start) : Base(start), _file(file)
		{ }

		template <typename Derive>
		DiskPos(DiskPos<Derive> const& that) : Base(that), _file(that._file)
		{ }

		shared_ptr<T> ReadObject() const
		{
			return _file->Read<T>(shared_from_this());// 这里还要 shared_from_this 吗？
		}

		// shared_ptr<File> GetFile() const
		// {
		// 	return _file;
		// }
	};
}