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
	using ::Collections::Btree;
	using ::Collections::LeafNode;
	using ::Collections::MiddleNode;
	using ::Collections::NodeBase;
	using ::Collections::order_int;
	using ::std::array;
	using ::std::byte;
	using ::std::copy;
	using ::std::declval;
	using ::std::make_shared;
	using ::std::move;
	using ::std::shared_ptr;
	using ::std::size_t;
	using ::std::vector;

	template <typename T>
	struct ByteConverter<DiskPtr<T>, false>
	{
		using ThisType = DiskPtr<T>;
		static constexpr size_t Size = ByteConverter<decltype(declval<ThisType>()._pos)>::Size;

		static array<byte, Size> ConvertToByte(ThisType const& p)
		{
			return ByteConverter<decltype(p._pos)>::ConvertToByte(p._pos);
		}

		static ThisType ConvertFromByte(shared_ptr<File> file, uint32_t startInFile)
		{
			return ByteConverter<decltype(declval<ThisType>()._pos)>::ConvertFromByte(file, startInFile);
		}
	};

	template <typename T>
	struct ByteConverter<WeakDiskPtr<T>, false>
	{
		using ThisType = WeakDiskPtr<T>;
		static constexpr size_t Size = ByteConverter<decltype(declval<ThisType>()._pos)>::Size;

		static array<byte, Size> ConvertToByte(ThisType const& p)
		{
			return ByteConverter<decltype(p._pos)>::ConvertToByte(p._pos);
		}

		// TODO �������ֲ����Ľӿڿ��ܻ�ģ���Ϊ������������ Pos �Ĳ�����
		static ThisType ConvertFromByte(shared_ptr<File> file, uint32_t startInFile)
		{
			return ByteConverter<decltype(declval<ThisType>()._pos)>::ConvertFromByte(file, startInFile);
		}
	};

	// Btree Ҫ�������� friend��ByteConverter �� TypeConverter������������
	template <typename Key, typename Value, order_int Order>
	struct ByteConverter<Btree<Order, Key, Value, DiskPtr>, false>
	{
		using ThisType = Btree<Order, Key, Value, DiskPtr>;

		static auto ConvertToByte(ThisType const& t)
		{
#define CONVERT_UNIT(OBJ) ByteConverter<decltype(OBJ)>::ConvertToByte(OBJ)
			return CONVERT_UNIT(t._keyCount) + CONVERT_UNIT(t._root);
#undef CONVERT_UNIT
		}

		static ThisType ConvertFromByte(shared_ptr<File> file, uint32_t startInFile)
		{
#define CONVERT_UNIT(PROPERTY) ByteConverter<decltype(declval<ThisType>().PROPERTY)>::ConvertFromByte(file, startInFile)
			auto c = CONVERT_UNIT(_keyCount);
			startInFile += ByteConverter<decltype(declval<ThisType>()._keyCount)>::Size;
			auto r = CONVERT_UNIT(_root);
			return { c, move(r) };// TODO set callback inner
#undef CONVERT_UNIT
		}

		static constexpr size_t Size = ConvertedByteSize<ThisType>;
	};

	template <typename Key, typename Value, order_int Count>
	struct ByteConverter<MiddleNode<Key, Value, Count, DiskPtr>, false>
	{
		using ThisType = MiddleNode<Key, Value, Count, DiskPtr>;
		static constexpr size_t Size = ByteConverter<decltype(declval<ThisType>()._elements)>::Size;

		static array<byte, Size> ConvertToByte(ThisType const& t)
		{
			return ByteConverter<decltype(t._elements)>::ConvertToByte(t._elements);
		}

		static ThisType ConvertFromByte(shared_ptr<File> file, uint32_t startInFile)
		{
			using T = decltype(declval<ThisType>()._elements);
			auto elements = ByteConverter<T>::ConvertFromByte(file, startInFile);
			return { move(elements) };// TODO provide LeafNode previous, next and callback inner
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
			// copy ���Զ����� vector ����� size ��Ӧ�ò���ֱ���㿽��
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

		static shared_ptr<ThisType> ConvertFromByte(shared_ptr<File> file, uint32_t startInFile)
		{
			auto middle = file->Read<bool>(startInFile, sizeof(bool));
			auto contentStart = startInFile + sizeof(middle);

			if (middle)
			{
				return make_shared<MidNode>(ByteConverter<MidNode>::ConvertFromByte(file, contentStart));
			}
			else
			{
				return make_shared<LeafNode>(ByteConverter<LeafNode>::ConvertFromByte(file, contentStart));
			}
		}
	};
}