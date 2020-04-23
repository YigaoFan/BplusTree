#pragma once
#include <filesystem>
#include <fstream>
#include <vector>
#include <array>
#include <cstddef>
#include <functional>
#include <string>
#include <memory>
#include <utility>

namespace FuncLib
{
	using ::std::filesystem::path;
	using ::std::move;
	using ::std::ifstream;
	using ::std::ofstream;
	using ::std::fstream;
	using ::std::vector;
	using ::std::array;
	using ::std::pair;
	using ::std::byte;
	using ::std::function;
	using ::std::string;
	using ::std::move;
	using ::std::size_t;
	using ::std::ios_base;
	using ::std::enable_shared_from_this;
	using ::std::shared_ptr;

	// Need to change, if on different PC
	constexpr uint32_t DiskBlockSize = 4096;

	// 这里其实不一定是文件，比如它可以存在另外一台机器上，通过网络通信来实现这样的效果
	class File : public enable_shared_from_this<File>
	{
	private:
		path _filename;
		size_t _currentPos;
		vector<pair<uint32_t, vector<byte>>> _bufferQueue;
	public:
		File(path filename, size_t startPos = 0);
		shared_ptr<File> GetPtr();
		void Flush();
		vector<byte> Read(uint32_t start, uint32_t size);
		~File();

		// vector 和 array 是不是应该写在不同的区，两个稳定程度不一样，array 变化还可以写在原地，除非析构了
		template <typename T>// maybe future use
		size_t Allocate(vector<byte> writeData)
		{
			auto pos = _currentPos;
			_currentPos += writeData.size();
			_bufferQueue.push_back({ pos, move(writeData) });
			return pos;
		}

		template <typename T>// maybe future use
		size_t Allocate(size_t size)
		{
			auto pos = _currentPos;
			_currentPos += size;
			return pos;
		}

		template <auto Size>
		size_t Allocate(array<byte, Size> writeData)
		{
			auto pos = _currentPos;
			_currentPos += Size;
			_bufferQueue.push_back({ pos, { writeData.begin(), writeData.end() } });
			return pos;
		}

		template <typename T>
		T Read(uint32_t start, uint32_t size)
		{
			// TODO test
			ifstream fs(_filename, ifstream::binary | ifstream::in);
			fs.seekg(start);

			byte mem[sizeof(T)];
			char c;
			T* const p = reinterpret_cast<T*>(&mem[0]);
			for (auto i = 0; i < size && fs.get(c); ++i)
			{
				mem[i] = c;
			}

			fs.close();
			return move(*p);
		}

		template <typename T>
		void Write(uint32_t start, vector<byte> data)
		{
			_bufferQueue.push_back({ start, move(data) });
		}

		template <typename T, auto Size>
		void Write(uint32_t start, array<byte, Size> data)
		{
			_bufferQueue.push_back({ start, { data.begin(), data.end() } });
		}
	private:
		void Filter();
	};
}