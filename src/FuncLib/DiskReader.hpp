#pragma once
#include <filesystem>
#include <fstream>

namespace FuncLib
{
	using ::std::filesystem::path;
	using ::std::move;
	using ::std::ifstream;

	class DiskReader
	{
	private:
		path _filePath;// TODO 这个 path 不用保存在硬盘里，因为就是硬盘正在读的这个文件

	public:
		DiskReader(path filePath)
			: _filePath(move(filePath))
		{ }

		template <typename T>
		T Read(uint32_t start, uint32_t size)
		{
			ifstream fs(_filePath, ifstream::binary | ifstream::in);
			char c;
			while (((start--) != 0) && fs.get(c))
			{ }

			char[sizeof(T)] _mem;
			T* const p = reinterpret_cast<T*>(&_mem[0]);
			for (auto i = 0; i < size && fs.get(c); ++i)
			{
				_mem[i] = c;
			}

			return move(*p);
		}
	};

	DiskReader::DiskReader(path filePath)
	{
	}
}