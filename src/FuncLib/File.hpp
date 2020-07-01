#pragma once
#include <filesystem>
#include <fstream>
#include <vector>
#include <array>
#include <cstddef>
#include <memory>
#include <utility>

namespace FuncLib
{
	using ::std::filesystem::path;
	using ::std::move;
	using ::std::ifstream;
	using ::std::ofstream;
	using ::std::vector;
	using ::std::array;
	using ::std::pair;
	using ::std::byte;
	using ::std::move;
	using ::std::size_t;
	using ::std::enable_shared_from_this;
	using ::std::shared_ptr;

	// Need to change, if on different PC
	constexpr uint32_t DiskBlockSize = 4096;
	using pos_int = size_t;

	template <typename T>
	concept Reader = requires(T t)
	{
		t.Read();// TODO add arg 
		t.Write();
	};

	// 这里其实不一定是文件，比如它可以存在另外一台机器上，通过网络通信来实现这样的效果
	class File : public enable_shared_from_this<File>
	{
	private:
		path _filename;
		pos_int _currentPos;
		vector<pair<pos_int, vector<byte>>> _bufferQueue;
	public:
		File(path filename, pos_int startPos = 0);
		void Flush();

		template <auto N>
		array<byte, N> Read(pos_int start)
		{
			Flush();
			ifstream fs(_filename, ifstream::binary);
			fs.seekg(start);

			array<byte, N> mem;

			if (fs.is_open())
			{
				fs.read(reinterpret_cast<char *>(&mem[0]), N);
			}

			return mem;
		}

		vector<byte> Read(pos_int start, size_t size);

		template <typename T>
		T Read(pos_int start, size_t size)
		{
			Flush();

			ifstream fs(_filename, ifstream::binary);
			fs.seekg(start);

			byte mem[sizeof(T)];
			if (fs.is_open())
			{
				fs.read(reinterpret_cast<char *>(&mem[0]), size);
			}

			T *p = reinterpret_cast<T *>(&mem[0]);
			return move(*p);
		}
		
		template <typename T>
		pos_int Allocate(size_t size)
		{
			auto pos = _currentPos;
			_currentPos += size;
			return pos;
		}

		// add a function to rewrite all object in disk memory to make reduce memory fragmentation

		void Deallocate(pos_int pos, size_t size)
		{
			Flush();
			// TODO help doc: https://zhuanlan.zhihu.com/p/29415507
		}
		
		template <typename T>
		pos_int Write(vector<byte> writeData)
		{
			auto pos = _currentPos;
			_currentPos += writeData.size();
			_bufferQueue.push_back({ pos, move(writeData) });
			return pos;
		}

		// vector 和 array 是不是应该写在不同的区，两个稳定程度不一样，array 变化还可以写在原地，除非析构了
		template <typename T, auto Size>
		pos_int Write(array<byte, Size> writeData)
		{
			auto pos = _currentPos;
			_currentPos += Size;
			_bufferQueue.push_back({ pos, { writeData.begin(), writeData.end() } });
			return pos;
		}

		template <typename T>
		void Write(pos_int start, vector<byte> data)
		{
			_bufferQueue.push_back({ start, move(data) });
		}

		template <typename T, auto Size>
		void Write(pos_int start, array<byte, Size> data)
		{
			_bufferQueue.push_back({ start, { data.begin(), data.end() } });
		}

		~File();
	private:
		void Filter();
	};
}