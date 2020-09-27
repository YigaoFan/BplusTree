#pragma once
#include <memory>
#include "Store/File.hpp"

namespace FuncLib
{
	using namespace Store;
	using ::std::shared_ptr;

	template <typename T>
	class DiskPos
	{
	private:
		friend struct ByteConverter<DiskPos, false>;
		template <typename>
		friend class DiskPos;

		File* _file;
		pos_lable _lable;
	public:
		DiskPos(File* file, pos_lable lable) : _file(file), _lable(lable)
		{ }

		template <typename Derive>
		DiskPos(DiskPos<Derive> const& that) : _file(that._file), _lable(that._lable)
		{ }

		shared_ptr<T> ReadObject() const
		{
			return _file->Read<T>(_lable);
		}

		void WriteObject(shared_ptr<T> const& obj) const
		{
			_file->Store(_lable, obj);
		}
	};
}