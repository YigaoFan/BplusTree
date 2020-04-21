#pragma once
#include <type_traits>
#include <string>
#include <vector>
#include <memory>
#include "../Btree/Elements.hpp"
#include "../Btree/NodeBase.hpp"
#include "../Btree/MiddleNode.hpp"
#include "../Btree/LeafNode.hpp"
#include "DiskPtr.hpp"
#include "File.hpp"

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

	template <typename T, bool = is_standard_layout_v<T> && is_trivial_v<T>>// condition may change
	struct TypeConverter// POD type
	{
		using From = T;
		using To = T;

		static Type ConvertFrom(T const& t, shared_ptr<File> file)// file is not must use, only for string like
		{
			return t;
		}
	};

	//template <typename T>
	//struct TypeConverter<T, false>// TODO may reduce
	//{
	//	using Type = DiskPtr<T>;

	//	static Type ConvertFrom(T const& t, shared_ptr<File> file)
	//	{
	//		return t;
	//	}
	//};

	//template <typename From>
	//struct TypeConverter
	//{
	//	using To = void;
	//	static To ConvertFrom(From const& from, shared_ptr<File> file)
	//	{
	//		static_assert(false, "Not support type convert");
	//	}
	//};

	// ֵ���ͺ�ָ������
	template <typename Key, typename Value, order_int Count>
	struct TypeConverter<Elements<Key, Value, Count>, false>
	// ����ĵڶ������Ͳ���ʹ�õ�ʱ�����ʡ����
	{
		// "������һ��Լ��"
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
			// TODO MiddleNode ��Ӧ�ü�һ������Ϊ elements �Ĺ��캯��
			using Key = Key;// TODO
			using Value = unique_ptr<NodeBase<Key, Value, Count, unique_ptr>>;
			return { TypeConverter<Elements<Key, Value, Count>>::ConvertFrom(from._elements, file) };
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
				using FromMidNode = MiddleNode<string, string, Count, unique_ptr>;
				return make_shared<To>(TypeConverter<FromMidNode>::ConvertFrom(static_cast<FromMidNode const&>(*from), file));
			}
			else
			{
				using FromLeafNode = LeafNode<string, string, Count, unique_ptr>;
				return make_shared<To>(TypeConverter<FromLeafNode>::ConvertFrom(static_cast<FromLeafNode const&>(*from), file);
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
			return To::MakeDiskPtr(TypeConverter<From>::ConvertFrom(from.get(), file), file);
		}
	};

	template <typename Key, typename Value, order_int Count>
	struct TypeConverter<Btree<Count, Key, Value, unique_ptr>, false>
	{
		using From = Btree<Count, Key, Value, unique_ptr>;
		using To = Btree<Count, Key, Value, DiskPtr>;

		static To ConvertFrom(From const& from, shared_ptr<File> file)
		{
			// TODO Btree ��Ӧ�ü�һ�������Ĺ��캯��
			return
			{ 
				from._keyCount, 
				TypeConverter<unique_ptr<NodeBase<Key, Value, Count, unique_ptr>>>::ConvertFrom(from, file) 
			};
		}
	};

	template <>
	struct TypeConverter<string>
	{
		using From = string;
		using To = DiskPtr<string>;

		static To ConvertFrom(From const& from, shared_ptr<File> file)
		{
			return file->Allocate<string>(ByteConverter<string>::ConvertToByte(from));
		}
	};
}