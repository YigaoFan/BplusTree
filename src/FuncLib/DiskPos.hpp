#pragma once
#include <filesystem>
#include <memory>
#include <map>
#include "DiskReader.hpp"

namespace FuncLib
{
	using ::std::shared_ptr;
	using ::std::map;

	template <typename T>
	class DiskPos
	{
	private:
		static map<uint32_t, shared_ptr<T>> _cache; // �� start Ӧ����û�µ�, if change, should delete

		uint32_t _start;
		uint32_t _size;
		//DiskReader _reader;
	public:
		// DiskPos ���� Converter ��ת���洢�Ͷ�ȡ�漰����ת��
		DiskPos(uint32_t start, uint32_t size)
			: _start(start), _size(size)
		{ }

		shared_ptr<T> ReadObject()
		{
			if (!this->_cache.contains(_start))
			{
				auto p = shared_ptr(_reader.Read<T>(_start, _size));
				_cache.insert({ _start, p });
				return p;
			}

			return this->_cache[_start];
		}

		void WriteObject()
		{

		}

		~DiskPos();

	private:

	};
}