#pragma once
#include <type_traits>
#include <algorithm>
#include <vector>
#include <array>
#include <string>
#include <memory>
#include <cstddef>
#include "../Btree/NodeBase.hpp"
#include "../Btree/MiddleNode.hpp"
#include "../Btree/LeafNode.hpp"
#include "DiskAllocator.hpp"
#include "DiskPtr.hpp"
#include "CurrentFile.hpp"

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
	using ::std::byte;
	using ::Collections::NodeBase;
	using ::Collections::LeafNode;
	using ::Collections::MiddleNode;
	
	enum ToPlace
	{
		Stack,
		Heap,
	};

	template <typename T>
	struct DiskDataConverter
	{
		static_assert(is_trivial_v<T> && is_standard_layout_v<T>,
			"Only support type which can use memory copy directly, "
			"Write a specialization for this template class to convert it to copyable");

		static array<byte, sizeof(T)> ConvertToDiskData(T t)
		{
			array<byte, sizeof(T)> mem;
			memcpy(&mem, &t, sizeof(T));
			return mem;
		}

		template <ToPlace Place = ToPlace::Heap>
		static auto ConvertFromDiskData(uint32_t startInFile)
		{
			array<byte, sizeof(T)> raw = CurrentFile::Read<T>(startInFile, sizeof(T));
			T* p = reinterpret_cast<T*>(&raw);

			if constexpr (Place == FuncLib::Stack)
			{
				return *p;
			}
			else
			{
				return make_shared<T>(*p);
			}
		}
	};

	template <>
	struct DiskDataConverter<string>
	{
		// TODO return value could be iterated
		vector<byte> ConvertToDiskData(string const& t)
		{
			return { t.begin(), t.end() };
		}

		shared_ptr<string> ConvertFromDiskData(uint32_t startInFile)
		{
			auto size = CurrentFile::Read<uint32_t>(startInFile, sizeof(uint32_t));
			auto contentStart = startInFile + sizeof(uint32_t);
			auto chars = CurrentFile::Read(contentStart, size);
			return make_shared<string>(chars.begin(), chars.end());
		}
	};

	using LibTreeLeaf = LeafNode<string, string, BtreeOrder>;
	using LibTreeMid = MiddleNode<string, string, BtreeOrder>;
	constexpr uint32_t UnitSize = 
		sizeof(LibTreeLeaf) > sizeof(LibTreeMid) ? sizeof(LibTreeLeaf) : sizeof(LibTreeMid);
	constexpr uint32_t BtreeOrder = DiskBlockSize / UnitSize;

	template <>
	struct DiskDataConverter<LibTreeMid>
	{
		using Middle = LibTreeMid;

		static array<byte, sizeof(T)> ConvertToDiskData(string const& t)
		{
			array<byte, sizeof(T)> mem;
			memcpy(&mem, &t, sizeof(T));
			return mem;
		}

		static shared_ptr<Middle> ConvertFromDiskData(uint32_t startInFile)
		{

		}
	};

	template <>
	struct DiskDataConverter<LibTreeLeaf>
	{
		using Leaf = LibTreeLeaf;

		static array<byte, sizeof(T)> ConvertToDiskData(string const& t)
		{
			array<byte, sizeof(T)> mem;
			memcpy(&mem, &t, sizeof(T));
			return mem;
		}

		static shared_ptr<Leaf> ConvertFromDiskData(uint32_t startInFile)
		{
		}
	};

	template <>
	struct DiskDataConverter<NodeBase<string, string, BtreeOrder>>
	{
		using Node = NodeBase<string, string, BtreeOrder>;

		static vector<byte> ConvertToDiskData(Node const& t)
		{
			
		}

		shared_ptr<Node> ConvertFromDiskData(uint32_t startInFile)
		{
			auto middle = CurrentFile::Read<bool>(startInFile, sizeof(bool));
			auto contentStart = startInFile + sizeof(bool);

			if (middle)
			{
				return DiskDataConverter<LibTreeMid>::ConvertFromDiskData(contentStart);
			}
			else
			{
				return DiskDataConverter<LibTreeLeaf>::ConvertFromDiskData(contentStart);
			}
		}
	};

	

	template <typename T>
	struct DiskDataConverter<DiskPos<T>>
	{
		using Pos = DiskPos<T>;
		using Index = typename Pos::Index;

		array<byte, sizeof(Pos)> ConvertToDiskData(Pos p)
		{
			return DiskDataConverter<Index>::ConvertToDiskData(p._start);
		}

		template <ToPlace Place = ToPlace::Heap>
		auto ConvertFromDiskData(uint32_t startInFile)
		{
			auto i = DiskDataConverter<Index>::ConvertFromDiskData<ToPlace::Stack>(p._start);
			if constexpr (Place == FuncLib::Stack)
			{
				return Pos(i);
			}
			else
			{
				return make_shared<Pos>(i);
			}
		}
	};

	template <typename T>
	struct DiskDataConverter<DiskPtr<T>>
	{
		using Ptr = DiskPtr<T>;

		auto ConvertToDiskData(Ptr& p)
		{
			return DiskDataConverter<decltype(p._pos)>::ConvertToDiskData(p._pos);
		}

		shared_ptr<Ptr> ConvertFromDiskData(uint32_t startInFile)
		{
			return make_shared<Ptr>(
				DiskDataConverter<decltype(p._pos)>::ConvertFromDiskData<ToPlace::Stack>(p._pos));
		}
	};

	template <typename T>
	struct DiskDataConverter<WeakDiskPtr<T>>
	{
		using Ptr = WeakDiskPtr<T>;

		auto ConvertToDiskData(Ptr& p)
		{
			return DiskDataConverter<decltype(p._pos)>::ConvertToDiskData(p._pos);
		}

		shared_ptr<Ptr> ConvertFromDiskData(uint32_t startInFile)
		{
			return make_shared<Ptr>(
				DiskDataConverter<decltype(p._pos)>::ConvertFromDiskData<ToPlace::Stack>(p._pos));
		}
	};
}