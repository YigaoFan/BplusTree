#pragma once
#include <filesystem>
#include <memory>

namespace FuncLib
{
	using ::std::filesystem::path;
	using ::std::enable_shared_from_this;
	using ::std::shared_ptr;
	using ::std::move;

	// Need to change, if on different PC
	constexpr uint32_t DiskBlockSize = 4096;
	enum DataType
	{
		Node,
		Other,
	};

	class CurrentFile // ������� allocator ����Ҫ�з�����Ϣ���� Reader ȥ��
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