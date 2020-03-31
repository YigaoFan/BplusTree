#pragma once
#include <type_traits>
#include <algorithm>
#include <vector>
#include <array>
#include <string>
#include <memory>
#include "../Btree/NodeBase.hpp"
#include "../Btree/MiddleNode.hpp"
#include "../Btree/LeafNode.hpp"
#include "DiskAllocator.hpp"
#include "DiskPtr.hpp"
#include "FileUtil.hpp"

namespace FuncLib
{
	using ::std::is_trivial_v;
	using ::std::is_standard_layout_v;
	using ::std::memcpy;
	using ::std::vector;
	using ::std::array;
	using ::std::string;
	using ::std::shared_ptr;
	using ::std::make_shared;
	using ::Collections::NodeBase;
	using ::Collections::LeafNode;
	using ::Collections::MiddleNode;

	// 不定大小的类型，比如 string，如何保留大小信息读取
	template <typename T>
	struct DiskDataConverter
	{
		static_assert(is_trivial_v<T> && is_standard_layout_v<T>,
			"Only support type which can use memory copy directly, "
			"Write a specialization for this template class to convert it to copyable");

		// TODO should be a pair, pointer and count
		static array<uint8_t, sizeof(T)> ConvertToDiskData(T t)
		{
			array<uint8_t, sizeof(T)> mem;
			memcpy(&mem, &t, sizeof(T));
			return mem;
		}

		/*template <auto Count>
		static T ConvertFromDiskData(char const(&raw)[Count])
		{
			return reinterpret_cast<T>(raw);
		}*/

		static T ConvertFromDiskData(uint32_t startInFile)
		{
			array<uint8_t, sizeof(T)> raw = FileUtil::Read<T>(startInFile, sizeof(T));
			T* p = reinterpret_cast<T*>(&raw);
			return *p;
		}
	};

	template <>
	struct DiskDataConverter<string>
	{
		vector<uint8_t> ConvertToDiskData(string const& t)
		{
			return { t.begin(), t.end() };
		}

		shared_ptr<string> ConvertFromDiskData(uint32_t startInFile)
		{
			auto size = FileUtil::Read<uint32_t>(startInFile, sizeof(uint32_t));
			auto contentStart = startInFile + sizeof(uint32_t);
			auto chars = FileUtil::Read(contentStart, size);
			return make_shared<string>(chars.begin(), chars.end());
		}
	};

	using LibBtreeLeaf = LeafNode<string, string, BtreeOrder>;
	using LibBtreeMid = MiddleNode<string, string, BtreeOrder>;
	constexpr uint32_t BigOne = 
		sizeof(LibBtreeLeaf) > sizeof(LibBtreeMid) ? sizeof(LibBtreeLeaf) : sizeof(LibBtreeMid);
	constexpr uint32_t BtreeOrder = DiskBlockSize / BigOne;

	template <>
	struct DiskDataConverter<NodeBase<string, string, BtreeOrder>>
	{
		using T = NodeBase<string, string, BtreeOrder>;

		T* ConvertFromDiskData(array<uint8_t, sizeof(T)> raw)
		{
			T* p = reinterpret_cast<T*>(&raw);
			return p;
		}
	};

	template <>
	struct DiskDataConverter<MiddleNode<string, string, BtreeOrder>>
	{
		using T = MiddleNode<string, string, BtreeOrder>;

		array<uint8_t, sizeof(T)> ConvertToDiskData(string const& t)
		{
			array<uint8_t, sizeof(T)> mem;
			memcpy(&mem, &t, sizeof(T));
			return mem;
		}

		T ConvertFromDiskData(array<uint8_t, sizeof(T)> raw)
		{
			T* p = reinterpret_cast<T*>(&raw);
			return *p;
		}
	};

	template <>
	struct DiskDataConverter<NodeBase<string, string, BtreeOrder>>
	{
		using T = LeafNode<string, string, BtreeOrder>;

		array<uint8_t, sizeof(T)> ConvertToDiskData(string const& t)
		{
			array<uint8_t, sizeof(T)> mem;
			memcpy(&mem, &t, sizeof(T));
			return mem;
		}

		T ConvertFromDiskData(array<uint8_t, sizeof(T)> raw)
		{
			T* p = reinterpret_cast<T*>(&raw);
			return *p;
		}
	};

	template <>
	struct DiskDataConverter<DiskPtr>
	{
		using T = DiskPtr;

		array<uint8_t, sizeof(T)> ConvertToDiskData(string const& t)
		{
			array<uint8_t, sizeof(T)> mem;
			memcpy(&mem, &t, sizeof(T));
			return mem;
		}

		T ConvertFromDiskData(array<uint8_t, sizeof(T)> raw)
		{
			T* p = reinterpret_cast<T*>(&raw);
			return *p;
		}
	};

	/*template <typename T>
	DiskDataConverter() -> DiskDataConverter<*/
	template <typename T>
	struct DiskDataConverter<DiskPos<T>>
	{
		using Pos = DiskPos<T>;

		array<uint8_t, sizeof(Pos)> ConvertToDiskData(Pos p)
		{
			array<uint8_t, sizeof(Pos)> mem;
			memcpy(&mem, &p, sizeof(Pos));
			return mem;
		}

		T ConvertFromDiskData(uint32_t startInFile)
		{
			using Index = typename Pos::Index;
			auto i = FileUtil::Read<Index>(startInFile, sizeof(Index));
			return Pos(i);
		}
	};
}