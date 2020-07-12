#pragma once
#include <type_traits>
#include <string>
#include <vector>
#include <memory>
#include "../Btree/Elements.hpp"
#include "../Btree/NodeBase.hpp"
#include "../Btree/MiddleNode.hpp"
#include "../Btree/LeafNode.hpp"
#include "../Btree/Btree.hpp"
#include "File.hpp"
#include "ByteConverter.hpp"

namespace FuncLib
{
	using ::std::string;
	using ::std::is_trivial_v;
	using ::std::is_standard_layout_v;
	using ::std::unique_ptr;
	using ::std::shared_ptr;
	using ::std::make_shared;
	using ::Collections::order_int;
	using ::Collections::Elements;
	using ::Collections::NodeBase;
	using ::Collections::LeafNode;
	using ::Collections::MiddleNode;
	using ::Collections::Btree;

	template <typename T, bool> // default value not declare here don't have problem? No problem, because include LeafNode up.
	struct TypeConverter
	{
		using From = T;
		using To = T;

		// file is not must use, only for string like
		static To ConvertFrom(From const& t, shared_ptr<File> file)
		{
			return t;
		}
	};

	template <typename Key, typename Value, order_int Count>
	struct TypeConverter<Elements<Key, Value, Count>, false>
	{
		using From = Elements<Key, Value, Count>;
		using To = Elements<typename TypeConverter<Key>::To, typename TypeConverter<Value>::To, Count>;

		static To ConvertFrom(From const& from, shared_ptr<File> file)
		{
			To to;
			for (auto& e : from)
			{
				to.Append({
					TypeConverter<Key>::ConvertFrom(e.first, file),
					TypeConverter<Value>::ConvertFrom(e.second, file)
				});
			}

			return to;
		}
	};

	// Ȼ���� DiskVer �Ķ���ת����Ӳ�����ݵĲ�����Ȼ��д��Ӳ�����ݣ��ͳ��� DiskPtr ��

	template <typename Key, typename Value, order_int Count>
	struct TypeConverter<MiddleNode<Key, Value, Count, unique_ptr>, false>
	{
		using From = MiddleNode<Key, Value, Count, unique_ptr>;
		using To = MiddleNode<Key, Value, Count, DiskPtr>;

		static To ConvertFrom(From const& from, shared_ptr<File> file)
		{
			using K = Key;
			using V = unique_ptr<NodeBase<Key, Value, Count, unique_ptr>>;
			return { TypeConverter<Elements<K, V, Count>>::ConvertFrom(from._elements, file) };
		}
	};

	template <typename Key, typename Value, order_int Count>
	struct TypeConverter<LeafNode<Key, Value, Count, unique_ptr>, false>
	{
		// "������һ��Լ��"���������ݳ�Ա����һ���ˣ��϶�����ͬһ������
		using From = LeafNode<Key, Value, Count, unique_ptr>;
		using To = LeafNode<Key, Value, Count, DiskPtr>;

		static To ConvertFrom(From const& from, shared_ptr<File> file)
		{
			// TODO LeafNode ��Ӧ�ü�һ������Ϊ elements �Ĺ��캯��
			return { TypeConverter<Elements<Key, Value, Count>>::ConvertFrom(from._elements, file) };
		}
	};

	template <typename Key, typename Value, order_int Count>
	struct TypeConverter<NodeBase<Key, Value, Count, unique_ptr>, false>
	{
		using From = NodeBase<Key, Value, Count, unique_ptr>;
		using To = NodeBase<Key, Value, Count, DiskPtr>;

		static shared_ptr<To> ConvertFrom(From const* from, shared_ptr<File> file)
		{
			if (from->Middle())
			{
				using FromMidNode = MiddleNode<Key, Value, Count, unique_ptr>;
				using ToMidNode = MiddleNode<Key, Value, Count, DiskPtr>;
				return make_shared<ToMidNode>(TypeConverter<FromMidNode>::ConvertFrom(static_cast<FromMidNode const &>(*from), file));
			}
			else
			{
				using FromLeafNode = LeafNode<Key, Value, Count, unique_ptr>;
				using ToLeafNode = LeafNode<Key, Value, Count, DiskPtr>;
				return make_shared<ToLeafNode>(TypeConverter<FromLeafNode>::ConvertFrom(static_cast<FromLeafNode const &>(*from), file));
			}
		}
	};

	template <typename Key, typename Value, order_int Count>
	struct TypeConverter<unique_ptr<NodeBase<Key, Value, Count, unique_ptr>>, false>
	{
		using From = unique_ptr<NodeBase<Key, Value, Count, unique_ptr>>;
		using To = DiskPtr<NodeBase<Key, Value, Count, DiskPtr>>;

		static To ConvertFrom(From const& from, shared_ptr<File> file)
		{
			auto p = from.get();
			return To::MakeDiskPtr(TypeConverter<typename From::element_type>::ConvertFrom(p, file), file);
		}
	};

	template <typename Key, typename Value, order_int Count>
	struct TypeConverter<Btree<Count, Key, Value, unique_ptr>, false>
	{
		using From = Btree<Count, Key, Value, unique_ptr>;
		using To = Btree<Count, Key, Value, DiskPtr>;

		static To ConvertFrom(From const& from, shared_ptr<File> file)
		{
			return
			{ 
				from._keyCount, 
				TypeConverter<unique_ptr<NodeBase<Key, Value, Count, unique_ptr>>>::ConvertFrom(from._root, file) 
			};
		}
	};

	template <>
	struct TypeConverter<string>
	{
		using From = string;
		using To = DiskRef<string>;

		// ����Ĳ����о������棬�����������������ôʵ�ֵ�
		static To ConvertFrom(From const& from, shared_ptr<File> file)
		{
			return DiskPtr<From>::MakeDiskPtr(make_shared<From>(from), file);
		}
	};
}