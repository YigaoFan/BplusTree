#pragma once
#include <memory>
#include <type_traits>
#include "Store/IInsidePositionOwner.hpp"

namespace FuncLib
{
	using namespace Store;
	using ::std::enable_if_t;
	using ::std::is_abstract_v;
	using ::std::make_shared;
	using ::std::shared_ptr;
	using ::std::size_t;

	class File;

	template <typename T>
	class DiskPos : public IInsidePositionOwner
	{
	private:
		friend struct ByteConverter<DiskPos, false>;
		template <typename Ty>
		friend class DiskPos;

		shared_ptr<File> _file;
		size_t _start;
	public:
		using Index = decltype(_start);

		DiskPos(shared_ptr<File> file, size_t start) : _start(start), _file(file)
		{ }

		template <typename Derive>
		DiskPos(DiskPos<Derive> const& that) : _file(that._file), _start(that._start)
		{ }

		void Addr(pos_int newPos) override
		{
			this->_start = newPos;
		}

		pos_int Addr() const override
		{
			return this->_start;
		}

		size_t RequiredSize() const override
		{
			return ByteConverter<T>::ConvertToByte(*entity).size();
		}

		shared_ptr<T> ReadObject() const
		{
			if constexpr (is_abstract_v<T>)
			{
				// ConvertFromByte need to ensure return a shared_ptr
				return ByteConverter<T>::ConvertFromByte(_file, _start);
			}
			else
			{
				return make_shared<T>(ByteConverter<T>::ConvertFromByte(_file, _start));
			}
		}

		void WriteObject(shared_ptr<T> entity)
		{
			// ������Ҫ�ݹ�д����������
			_file->Write<T>(ByteConverter<T>::ConvertToByte(*entity));
		}

		shared_ptr<File> GetFile() const
		{
			return _file;
		}
	};
}