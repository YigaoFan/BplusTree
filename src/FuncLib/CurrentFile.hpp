#pragma once
#include <filesystem>
#include <fstream>
#include <vector>
#include <cstddef>

namespace FuncLib
{
	using ::std::filesystem::path;
	using ::std::move;
	using ::std::ifstream;
	using ::std::vector;
	using ::std::byte;

	// Need to change, if on different PC
	constexpr uint32_t DiskBlockSize = 4096;

	enum DataType
	{
		Node,
		Other,
	};

	class CurrentFile // 这里或者 allocator 里面要有分区信息，供 Reader 去读
	{
	private:
		static path& Path()
		{
			thread_local path p;
			return p;
		}

	public:
		static void SetPath(path p)
		{
			Path() = p;
		}

		static path GetPath()
		{
			return Path();
		}

		template <typename T>
		static T Read(uint32_t start, uint32_t size)
		{
			auto p = GetPath();
			ifstream fs(p, ifstream::binary | ifstream::in);
			char c;
			while (((start--) != 0) && fs.get(c))
			{
			}

			byte[sizeof(T)] mem;
			T* const p = reinterpret_cast<T*>(&mem[0]);
			for (auto i = 0; i < size && fs.get(c); ++i)
			{
				mem[i] = c;
			}

			return move(*p);
		}

		static vector<byte> Read(uint32_t start, uint32_t size)
		{
			auto p = GetPath();
			ifstream fs(p, ifstream::binary | ifstream::in);
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
		static void Write(uint32_t start, uint32_t size)
		{
			// TODO
		}
	};
}