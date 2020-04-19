#pragma once
#include <filesystem>
#include <fstream>
#include <vector>
#include <cstddef>
#include <functional>
#include <string>
#include <memory>
#include "DiskPos.hpp"

namespace FuncLib
{
	using ::std::filesystem::path;
	using ::std::move;
	using ::std::ifstream;
	using ::std::ofstream;
	using ::std::vector;
	using ::std::byte;
	using ::std::function;
	using ::std::string;
	using ::std::move;
	using ::std::size_t;
	using ::std::ios_base;
	using ::std::enable_shared_from_this;

	// Need to change, if on different PC
	constexpr uint32_t DiskBlockSize = 4096;

	// 这里其实不一定是文件，比如它可以存在另外一台机器上，通过网络通信来实现这样的效果
	class File : public enable_shared_from_this<File>
	{
	private:
		path _filename;
		size_t _currentPos;
		vector<vector<byte>> _bufferQueue;
		bool _hasFlush = false;
	public:
		File(path filename, size_t startPos = 0) : _filename(move(filename)), _currentPos(startPos)
		{ }
		
		shared_ptr<File> GetPtr()
		{
			return shared_from_this();
		}

		// vector 和 array 是不是应该写在不同的区，两个稳定程度不一样，array 变化还可以写在原地，除非析构了
		template <typename T>
		DiskPos<T> Allocate(vector<byte> writeData)
		{
			_currentPos += writeData.size();
			auto pos = _currentPos;
			_bufferQueue.push_back(move(writeData));
			return { GetPtr(), pos };
		}

		template <typename T, auto Size>
		DiskPos<T> Allocate(array<byte, Size> writeData)
		{
			auto pos = _currentPos;
			_currentPos += Size;
			_bufferQueue.push_back({ wirteData.begin(), wirteData.end() });
			return { GetPtr(), pos };
		}

		void Flush()
		{
			if (!_hasFlush)
			{
				ofstream fs(_filename, ios_base::out | ios_base::binary);
				for (auto& bytes : _bufferQueue)
				{
					// char not ensure to equal to byte
					fs.write((char*)(&bytes[0]), bytes.size());
				}

				fs.close();
			}
		}

		template <typename T>
		T Read(uint32_t start, uint32_t size)
		{
			ifstream fs(_filename, ifstream::binary | ifstream::in);
			char c;
			while (((start--) != 0) && fs.get(c))
			{ }

			byte[sizeof(T)] mem;
			T* const p = reinterpret_cast<T*>(&mem[0]);
			for (auto i = 0; i < size && fs.get(c); ++i)
			{
				mem[i] = c;
			}

			return move(*p);
		}

		vector<byte> Read(uint32_t start, uint32_t size)
		{
			ifstream fs(_filename, ifstream::binary | ifstream::in);
			char c;
			while (((start--) != 0) && fs.get(c))
			{
			}

			vector<byte> mem;
			mem.reserve(size);

			for (auto i = 0; i < size && fs.get(c); ++i)
			{
				mem.push_back(static_cast<byte>(c));
			}

			return mem;
		}

		template <typename T>
		void Write()
		{
			// TODO
		}

		// 有了 File 之后，如何书写在原来分配的位置

		~File()
		{
			Flush();
		}
	};
}