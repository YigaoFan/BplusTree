#pragma once
#include <filesystem>
#include <memory>

namespace FuncLib
{
	using ::std::filesystem::path;
	using ::std::enable_shared_from_this;
	using ::std::shared_ptr;
	using ::std::move;

	class File
	{
	private:
		static path& Path(path p)
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

	template <typename T>
	class DiskPos
	{
	private:

	public:
		DiskPos();
		T Read();
		~DiskPos();

	private:

	};

	DiskPos::DiskPos()
	{
	}

	DiskPos::~DiskPos()
	{
	}
	class DiskAllocator : public enable_shared_from_this<DiskAllocator>
	{
	public:
		DiskAllocator(path);
		~DiskAllocator();

		template <typename T>
		DiskPos<T> Allocate(T const& t)// TODO decay T
		{

		}

		shared_ptr<DiskAllocator> GetPtr()
		{
			return shared_from_this();
		}
	private:

	};

	DiskAllocator::DiskAllocator(path)
	{
	}

	DiskAllocator::~DiskAllocator()
	{
	}
}