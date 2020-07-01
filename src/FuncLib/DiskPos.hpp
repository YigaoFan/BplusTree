#pragma once
#include <map>
#include <memory>
#include "File.hpp"

namespace FuncLib
{
	using ::std::map;
	using ::std::size_t;
	using ::std::shared_ptr;

	template <typename T>
	class DiskPos
	{
	private:
		friend struct ByteConverter<DiskPos, false>;
		static map<size_t, shared_ptr<T>> _cache; // 用 start 应该是没事的, if change, should delete

		shared_ptr<File> _file;
		size_t _start;
	public:
		DiskPos(shared_ptr<File> file, size_t start) : _start(start), _file(file)
		{ }

		shared_ptr<T> ReadObject()
		{
			return ByteConverter<T>::ConvertFromDiskData(_file, _start);
			//if (!this->_cache.contains(_start))
			//{
			//	auto p = shared_ptr(CurrentFile::Read(_start, _size));
			//	_cache.insert({ _start, p });
			//	return p;
			//}

			//return this->_cache[_start];
		}

		void WriteObject(shared_ptr<T> entity)
		{
			// 这里需要递归写入子内容吗
			_file->Write<T>(ByteConverter<T>::ConvertToByte(*entity));
		}

		shared_ptr<File> GetFile() const
		{
			return _file;
		}

		~DiskPos();
	};
}