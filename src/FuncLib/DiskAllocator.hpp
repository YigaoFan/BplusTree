#pragma once
#include <filesystem>
#include <memory>

namespace FuncLib
{
	using ::std::filesystem::path;
	using ::std::enable_shared_from_this;
	using ::std::shared_ptr;

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