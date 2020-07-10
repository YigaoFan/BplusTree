#pragma once
#include <map>
#include <memory>
#include <type_traits>
#include "File.hpp"

namespace FuncLib
{
	using ::std::enable_if_t;
	using ::std::is_abstract_v;
	using ::std::is_base_of_v;
	using ::std::make_shared;
	using ::std::map;
	using ::std::shared_ptr;
	using ::std::size_t;

	template <typename T>
	class DiskPos
	{
	private:
		friend struct ByteConverter<DiskPos, false>;
		template <typename Ty>
		friend class DiskPos;
		static map<size_t, shared_ptr<T>> _cache; // �� start Ӧ����û�µ�, if change, should delete

		shared_ptr<File> _file;
		size_t _start;
	public:
		using Index = decltype(_start);

		DiskPos(shared_ptr<File> file, size_t start) : _start(start), _file(file)
		{ }

		template <typename Derive, typename = enable_if_t<is_base_of_v<T, Derive>>>
		DiskPos(DiskPos<Derive> const& that) : _file(that._file), _start(that._start)
		{ }

		shared_ptr<T> ReadObject()
		{
			if constexpr (is_abstract_v<T>)
			{
				return ByteConverter<T>::ConvertFromByte(_file, _start); // ConvertFromByte need to ensure return a shared_ptr
			}
			else
			{
				return make_shared<T>(ByteConverter<T>::ConvertFromByte(_file, _start));
			}
			
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
			// ������Ҫ�ݹ�д����������
			_file->Write<T>(ByteConverter<T>::ConvertToByte(*entity));
		}

		shared_ptr<File> GetFile() const
		{
			return _file;
		}

		// ~DiskPos(); will use?
	};
}