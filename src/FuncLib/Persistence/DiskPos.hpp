#pragma once
#include <memory>
#include <utility>
#include "../Store/File.hpp"
#include "IWriterIReaderConcept.hpp"

namespace FuncLib::Persistence
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
		pos_label _label;
	public:
		DiskPos() : _file(nullptr), _label(0)
		{ }

		DiskPos(File* file, pos_label label) : _file(file), _label(label)
		{ }

		template <typename Derived>
		DiskPos(DiskPos<Derived> const& that) : _file(that._file), _label(that._label)
		{ }

		shared_ptr<T> ReadObject() const
		{
			return _file->Read<T>(_label);
		}

		void WriteObject(shared_ptr<T> const& obj, IWriter auto* writer) const
		{
			_file->StoreInner(_label, obj, writer);
		}

		pair<DiskPos, shared_ptr<T>> Clone(shared_ptr<T> const& obj) const
		{
			auto [label, clonedObj] = _file->New(*obj);
			return { DiskPos(_file, label), clonedObj };
		}

		File* GetLessOwnershipFile() const
		{
			return _file;
		}

		void RegisterSetter(function<void(T*)> setter) const
		{
			_file->RegisterSetter(_label, move(setter));
		}
	};

	template <typename T1, typename T2>
	bool operator== (DiskPos<T1> const& lhs, DiskPos<T2> const& rhs)
	{
		return lhs._file == rhs._file && lhs._label == rhs._label;
	}
}