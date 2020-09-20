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
		friend struct ByteConverter<DiskPos, false>;
		template <typename>
		friend class DiskPos;

		File* _file;
		pos_int _start;
	public:
		using Index = decltype(_start);

		DiskPos(File* file, pos_int start) : _start(start), _file(file)
		{ }

		template <typename Derive>
		DiskPos(DiskPos<Derive> const& that) : _file(that._file), _start(that._start)
		{ }

		shared_ptr<T> ReadObject() const
		{
			return _file->Read<T>(shared_from_this());
		}

		shared_ptr<File> GetFile() const
		{
			return _file;
		}
	};
}