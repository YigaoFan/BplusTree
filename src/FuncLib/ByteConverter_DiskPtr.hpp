#pragma once
#include <type_traits>
#include <vector>
#include <array>
#include <utility>
#include <memory>
#include <cstddef>
#include "../Btree/Basic.hpp"
#include "../Btree/Btree.hpp"
#include "../Btree/NodeBase.hpp"
#include "../Btree/MiddleNode.hpp"
#include "../Btree/LeafNode.hpp"

namespace FuncLib
{
	using ::std::declval;
	using ::std::copy;
	using ::std::vector;
	using ::std::array;
	using ::std::shared_ptr;
	using ::std::byte;
	using ::std::size_t;
	using ::std::move;
	using ::Collections::order_int;
	using ::Collections::Btree;
	using ::Collections::NodeBase;
	using ::Collections::LeafNode;
	using ::Collections::MiddleNode;

	template <typename T>
	struct ByteConverter<DiskPtr<T>, false>
	{
		using ThisType = DiskPtr<T>;
		static constexpr size_t Size = ByteConverter<decltype(declval<ThisType>()._pos)>::Size;

		array<byte, Size> ConvertToByte(ThisType const& p)
		{
			return ByteConverter<decltype(p._pos)>::ConvertToByte(p._pos);
		}

		ThisType ConvertFromByte(shared_ptr<File> file, uint32_t startInFile)
		{
			return ByteConverter<decltype(declval<ThisType>()._pos)>::ConvertFromByte(file, startInFile);
		}
	};

	template <typename T>
	struct ByteConverter<WeakDiskPtr<T>, false>
	{
		using ThisType = WeakDiskPtr<T>;
		static constexpr size_t Size = ByteConverter<decltype(declval<ThisType>()._pos)>::Size;

		array<byte, Size> ConvertToByte(ThisType const& p)
		{
			return ByteConverter<decltype(p._pos)>::ConvertToByte(p._pos);
		}

		// TODO 下面这种操作的接口可能会改，因为本来就是来自 Pos 的操作吗
		ThisType ConvertFromByte(shared_ptr<File> file, uint32_t startInFile)
		{
			return ByteConverter<decltype(declval<ThisType>()._pos)>::ConvertFromByte(file, startInFile);
		}
	};

	// Btree 要对两个类 friend，ByteConverter 和 TypeConverter。其它类类似
	template <typename Key, typename Value, order_int Order>
	struct ByteConverter<Btree<Order, Key, Value, DiskPtr>, false>
	{
		using ThisType = Btree<Order, Key, Value, DiskPtr>;
		struct DiskBtree
		{
			decltype(declval<ThisType>()._keyCount) KeyCount;
			decltype(declval<ThisType>()._root) Root;
		};

		static constexpr size_t Size = ByteConverter<DiskBtree>::Size;

		static array<byte, Size> ConvertToByte(ThisType const& t)
		{
			return ByteConverter<DiskBtree>::ConvertToByte({ t._keyCount, t._root });
		}

		static ThisType ConvertFromByte(shared_ptr<File> file, uint32_t startInFile)
		{
			auto t = ByteConverter<DiskBtree>::ConvertFromByte(file, startInFile);
			return { t.KeyCount, move(t.Root) };// TODO set callback inner
		}
	};

	template <typename Key, typename Value, order_int Count>
	struct ByteConverter<MiddleNode<Key, Value, Count, DiskPtr>, false>
	{
		using ThisType = MiddleNode<Key, Value, Count, DiskPtr>;
		static constexpr size_t Size = ByteConverter<decltype(declval<ThisType>()._elements)>::Size;

		static vector<byte> ConvertToByte(ThisType const& t)
		{
			return ByteConverter<decltype(t._elements)>::ConvertToByte(t._elements);
		}

		static ThisType ConvertFromByte(shared_ptr<File> file, uint32_t startInFile)
		{
			using T = decltype(declval<ThisType>()._elements);
			auto elements = ByteConverter<T>::ConvertFromByte(file, startInFile);
			return { move(elements) };// provide LeafNode previous, next and callback inner
		}
	};



	template <typename Key, typename Value, order_int Count>
	struct ByteConverter<LeafNode<Key, Value, Count, DiskPtr>, false>
	{
		using ThisType = LeafNode<Key, Value, Count, DiskPtr>;
		static constexpr size_t Size = ByteConverter<decltype(declval<ThisType>()._elements)>::Size;

		static array<byte, Size> ConvertToByte(ThisType const& t)
		{
			return ByteConverter<decltype(t._elements)>::ConvertToByte(t._elements);
		}

		static ThisType ConvertFromByte(shared_ptr<File> file, uint32_t startInFile)
		{
			using T = decltype(declval<ThisType>()._elements);
			auto elements = ByteConverter<T>::ConvertFromByte(file, startInFile);
			return { move(elements) };
		}
	};

	template <typename Key, typename Value, order_int Count>
	struct ByteConverter<NodeBase<Key, Value, Count, DiskPtr>, false>
	{
		using ThisType = NodeBase<Key, Value, Count, DiskPtr>;
		using MidNode = MiddleNode<Key, Value, Count, DiskPtr>;
		using LeafNode = LeafNode<Key, Value, Count, DiskPtr>;

		static vector<byte> ConvertToByte(ThisType const& node)
		{
			vector<byte> bytes;
			auto middle = node.Middle();
			auto arr = ByteConverter<bool>::ConvertToByte(middle);
			// copy 会自动扩充 vector 里面的 size 吗，应该不是直接裸拷吧
			copy(arr.begin(), arr.end(), bytes.end());

			if (middle)
			{
				auto byteArray = ByteConverter<MidNode>::ConvertToByte(static_cast<MidNode const&>(node));
				copy(byteArray.begin(), byteArray.end(), bytes.end());
				return bytes;
			}
			else
			{
				auto byteArray = ByteConverter<LeafNode>::ConvertToByte(static_cast<LeafNode const&>(node));
				copy(byteArray.begin(), byteArray.end(), bytes.end());
				return bytes;
			}
		}

		shared_ptr<ThisType> ConvertFromByte(shared_ptr<File> file, uint32_t startInFile)
		{
			auto middle = file->Read<bool>(startInFile, sizeof(bool));
			auto contentStart = startInFile + sizeof(middle);

			if (middle)
			{
				return ByteConverter<MidNode>::ConvertFromByte(file, contentStart);
			}
			else
			{
				return ByteConverter<LeafNode>::ConvertFromByte(file, contentStart);
			}
		}
	};
}