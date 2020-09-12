#pragma once
#include <memory>
#include "Store/IInsidePositionOwner.hpp"

namespace FuncLib
{
	using namespace Store;
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
		shared_ptr<T> _t;
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

		shared_ptr<T> ReadObject() const
		{
			if constexpr (is_abstract_v<T>)
			{
				// ConvertFromByte need to ensure return a shared_ptr
				_t = ByteConverter<T>::ConvertFromByte(_file, _start);// 这里要处理下，使读取出来的是指针还是对象本身
			}
			else
			{
				_t = make_shared<T>(ByteConverter<T>::ConvertFromByte(_file, _start));
			}

			return _t;
		}

		shared_ptr<File> GetFile() const
		{
			return _file;
		}
	};
}