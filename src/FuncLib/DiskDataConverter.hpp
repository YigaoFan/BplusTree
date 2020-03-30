#pragma once
#include <type_traits>
#include <algorithm>
#include <array>
#include <string>
#include "../Btree/NodeBase.hpp"
#include "../Btree/MiddleNode.hpp"
#include "../Btree/LeafNode.hpp"
#include "DiskAllocator.hpp"
#include "DiskPtr.hpp"

namespace FuncLib
{
	using ::std::is_trivial_v;
	using ::std::is_standard_layout_v;
	using ::std::memcpy;
	using ::std::array;
	using ::std::string;
	using ::Collections::NodeBase;
	using ::Collections::LeafNode;
	using ::Collections::MiddleNode;

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

		// TODO special support
		/*template <auto Count>
		static T ConvertFromDiskData(char const(&raw)[Count])
		{
			return reinterpret_cast<T>(raw);
		}*/

		static T ConvertFromDiskData(array<uint8_t, sizeof(T)> raw)
		{
			T* p = reinterpret_cast<T*>(&raw);
			return *p;
		}
	};

	template <>
	struct DiskDataConverter<string>
	{
		using T = string;
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

	constexpr uint32_t BtreeOrder = DiskBlockSize / 10; // TODO modify

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

	template <>
	struct DiskDataConverter<DiskPos>
	{
		using T = DiskPos;

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
}