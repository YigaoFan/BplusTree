#pragma once
#include <memory>
#include <utility>
#include "Store/File.hpp"

namespace FuncLib
{
	using namespace Store;
	using ::std::pair;
	using ::std::shared_ptr;

	template <typename T>
	class DiskPos
	{
	private:
		friend struct ByteConverter<DiskPos, false>;
		template <typename>
		friend class DiskPos;
		template <typename T2>
		friend bool operator== (DiskPos const& lhs, DiskPos<T2> const& rhs);

		File* _file;
		pos_lable _lable;
	public:
		DiskPos() : _file(nullptr), _lable(0)
		{ }

		DiskPos(File* file, pos_lable lable) : _file(file), _lable(lable)
		{ }

		template <typename T1>
		void AddSub(DiskPos<T1> const& subDiskPos)
		{
			_file->SetSubRelation(_lable, subDiskPos._lable);
		}

		template <typename Derived>
		DiskPos(DiskPos<Derived> const& that) : _file(that._file), _lable(that._lable)
		{ }

		shared_ptr<T> ReadObject() const
		{
			return _file->Read<T>(_lable);
		}

		void WriteObject(shared_ptr<T> const& obj) const
		{
			_file->Store(_lable, obj);
		}

		pair<DiskPos, shared_ptr<T>> Clone(shared_ptr<T> const& obj) const
		{
			auto [lable, clonedObj] = _file->New(*obj);
			return { DiskPos(_file, lable), clonedObj };
		}
	};

	template <typename T1, typename T2>
	bool operator== (DiskPos<T1> const& lhs, DiskPos<T2> const& rhs)
	{
		return lhs._file == rhs._file && lhs._lable == rhs._lable;
	}
}