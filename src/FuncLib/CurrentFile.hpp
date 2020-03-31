#pragma once
#include <filesystem>

namespace FuncLib
{
	using ::std::filesystem::path;

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
	};
}