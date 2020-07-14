#pragma once
#include <type_traits>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include "../Basic/TypeTrait.hpp"
#include "../Btree/Elements.hpp"
#include "../Btree/NodeBase.hpp"
#include "../Btree/MiddleNode.hpp"
#include "../Btree/LeafNode.hpp"
#include "../Btree/Btree.hpp"
#include "../Btree/EnumeratorPipeline.hpp"
#include "File.hpp"
#include "ByteConverter.hpp"

namespace FuncLib
{
	using ::Basic::ReturnType;
	using ::Collections::Btree;
	using ::Collections::Elements;
	using ::Collections::EnumeratorPipeline;
	using ::Collections::LeafNode;
	using ::Collections::MiddleNode;
	using ::Collections::NodeBase;
	using ::Collections::order_int;
	using ::std::declval;
	using ::std::is_standard_layout_v;
	using ::std::is_trivial_v;
	using ::std::make_shared;
	using ::std::shared_ptr;
	using ::std::string;
	using ::std::unique_ptr;

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

	template <typename Key, typename Value, order_int Count, typename LessThan>
	struct TypeConverter<Elements<Key, Value, Count, LessThan>, false>
	{
		using From = Elements<Key, Value, Count>;
		using To = Elements<typename TypeConverter<Key>::To, typename TypeConverter<Value>::To, Count, LessThan>;

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

	template <typename Key, typename Value, order_int Count>
	struct TypeConverter<MiddleNode<Key, Value, Count, unique_ptr>, false>
	{
		using From = MiddleNode<Key, Value, Count, unique_ptr>;
		using To = MiddleNode<Key, Value, Count, DiskPtr>;

		static DiskPtr<NodeBase<Key, Value, Count, DiskPtr>> CloneNodeToDisk(typename decltype(declval<From>()._elements)::Item const& item, shared_ptr<File> file)
		{
			return TypeConverter<decltype(item.second)>::ConvertFrom(item.second, file);
		}

		static To ConvertFrom(From const& from, shared_ptr<File> file)
		{
			using Node = NodeBase<Key, Value, Count, DiskPtr>;
			using ::std::placeholders::_1;
			// using ::std::placeholders::_2;
			auto nodes = EnumeratorPipeline<typename decltype(from._elements)::Item const&, DiskPtr<Node>>(from._elements.GetEnumerator(), bind(&CloneNodeToDisk, _1, file));
			return { move(nodes), from._elements.LessThanPtr };
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
			return { TypeConverter<Elements<Key, Value, Count>>::ConvertFrom(from._elements, file) };
		}
	};

	template <typename Key, typename Value, order_int Count>
	struct TypeConverter<NodeBase<Key, Value, Count, unique_ptr>, false>
	{
		using From = NodeBase<Key, Value, Count, unique_ptr>;
		using To = shared_ptr<NodeBase<Key, Value, Count, DiskPtr>>;

		static To ConvertFrom(From const& from, shared_ptr<File> file)
		{
			if (from.Middle())
			{
				using FromMidNode = MiddleNode<Key, Value, Count, unique_ptr>;
				using ToMidNode = MiddleNode<Key, Value, Count, DiskPtr>;
				return make_shared<ToMidNode>(TypeConverter<FromMidNode>::ConvertFrom(static_cast<FromMidNode const&>(from), file));
			}
			else
			{
				using FromLeafNode = LeafNode<Key, Value, Count, unique_ptr>;
				using ToLeafNode = LeafNode<Key, Value, Count, DiskPtr>;
				return make_shared<ToLeafNode>(TypeConverter<FromLeafNode>::ConvertFrom(static_cast<FromLeafNode const&>(from), file));
			}
		}
	};

	template <typename T>
	struct TypeConverter<unique_ptr<T>, false>
	{
		using From = unique_ptr<T>;

		static auto ConvertFrom(From const& from, shared_ptr<File> file)
		{
			auto p = from.get();
			auto c = TypeConverter<T>::ConvertFrom(*p, file);

			using ConvertedType = decltype(c);
			using ::Basic::IsSpecialization;
			if constexpr (IsSpecialization<ConvertedType, shared_ptr>::value)
			{
				return DiskPtr<typename ConvertedType::element_type>::MakeDiskPtr(c, file);
			}
			else if constexpr (IsSpecialization<ConvertedType, DiskPtr>::value)
			{
				return c;
			}
			else
			{
				return DiskPtr<ConvertedType>::MakeDiskPtr(make_shared<ConvertedType>(move(c)), file);
			}
		}

		using To = typename ReturnType<decltype(TypeConverter::ConvertFrom)>::Type;
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