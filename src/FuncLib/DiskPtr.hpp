#pragma once
#include <string>
#include <memory>
#include <type_traits>
#include "DiskPos.hpp"

namespace FuncLib
{
	using ::std::string;
	using ::std::shared_ptr;
	using ::std::move;
	using ::std::is_pod_v;
	using ::std::enable_if_t;
	// Btree should have a type para to pass pointer type
	// 要考虑全部持久化后，以 NodeBase 类型去构造内存里的 MiddleNode 和 LeafNode
	// 所以要支持某种转发机制，让 DiskPtr<NodeBase> 转发到 DiskPtr<MiddleNode> 或者 DiskPtr<LeafNode>

	template <typename T>
	class DiskPtrBase
	{
	private:
		shared_ptr<T> tPtr = nullptr;
		DiskPos<T> _pos;

	public:
		T* operator-> ()
		{
			if (tPtr == nullptr)
			{
				tPtr = _pos.ReadObject();
			}

			return tPtr;
		}
	};

	template <typename T>
	class WeakDiskPtr : public DiskPtrBase<T>
	{

	};

	template <typename T>
	class DiskPtr : public DiskPtrBase<T>
	{
	public:
		DiskPtr(DiskPtr& const) = delete;
		WeakDiskPtr<T> GetWeakPtr()
		{

		}
	};

	struct DiskBase
	{
		void WriteToDisk();
	};

	// POD, 写进去的都是 POD，非 POD 也要转换为 POD
	template <typename T, typename=enable_if_t<is_pod_v<T>>>
	class DiskPtr
	{
		void WriteToDisk();

	};

	// Not POD you should implement it like Node
	/*template <typename T, typename = enable_if_t<!is_pod_v<T>>>
	class DiskPtr
	{
		void WriteToDisk();

	};*/

	// For Node
	template <typename T>
	class DiskPtr
	{
		void WriteToDisk();

	};
	// Btree 中用到了几种数据？
	// 一种是实体，比如 Key，Value 最终在叶子节点存的都是实体，Node 之类应该是也是
	// 一种是存的指针，比如 unique_ptr，也就是说指针本身也要持久化
	template <typename T>
	class DiskPtr<DiskPtr<T>> // 即 DiskPtr 如何存在硬盘上
	{
		// 这里想到了一个问题，如何防止一个地方被重复读取
		// 即硬盘里的都是原始的，需要还原的，但每个都还原，就会重复了
		// 用缓存？
	};

	template <>
	class DiskPtr<string>
	{
	private:
		string* _src = nullptr;
		DiskPos<string> _pos;

	public:
		DiskPtr MakeDiskPtr(string s, shared_ptr<DiskAllocator> allocator)
		{
			auto p = allocator->Allocate(s);
			return DiskPtr(move(s), p);
		}

		string* operator-> ()
		{
			if (_src == nullptr)
			{
				_src = new string(_pos.ReadObject());
			}

			return _src;
		}

		~DiskPtr()
		{
			// Update content to disk
			// Or register update event in allocator, 然后集中更新
			// Use DiskPos to update
		}

	private:
		DiskPtr(string s, decltype(_pos) position)
			: _src(new string(move(s))), _pos(move(position))
		{

		}
	};

	// POD
	template <>
	class DiskPtr<int>
	{
	};
}