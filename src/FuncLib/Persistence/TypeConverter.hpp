#pragma once
#include <type_traits>
#include <string>
#include <vector>
#include <tuple>
#include <memory>
#include <utility>
#include <functional>
#include "../../Basic/TypeTrait.hpp"
#include "../../Btree/Elements.hpp"
#include "../../Btree/NodeBase.hpp"
#include "../../Btree/MiddleNode.hpp"
#include "../../Btree/LeafNode.hpp"
#include "../../Btree/Btree.hpp"
#include "../../Btree/EnumeratorPipeline.hpp"
#include "../Store/File.hpp"
#include "ByteConverter.hpp"
#include "OwnerState.hpp"

namespace FuncLib::Persistence
{
	using Basic::ReturnType;
	using Collections::Btree;
	using Collections::Elements;
	using Collections::EnumeratorPipeline;
	using Collections::LeafNode;
	using Collections::MiddleNode;
	using Collections::NodeBase;
	using Collections::order_int;
	using Collections::StorePlace;
	using ::std::declval;
	using ::std::get;
	using ::std::index_sequence;
	using ::std::make_shared;
	using ::std::pair;
	using ::std::shared_ptr;
	using ::std::string;
	using ::std::tuple;
	using ::std::tuple_element_t;
	using ::std::unique_ptr;
	using ::std::vector;

	template <typename T, OwnerState Owner>
	struct TypeConverter
	{
		using From = T;
		using To = T;

		// file is not must use, only for string like
		static To ConvertFrom(From const& t, File* file)
		{
			return t;
		}
	};

	template <typename T>
	struct TypeConverter<T*>
	{
		using From = T*;
		using To = OwnerLessDiskPtr<T>;

		// file is not must use, only for string like
		// convert cannot run if only with raw pointer
		// static To ConvertFrom(From const& t, File* file)
		// {
		// 	return t;
		// }
	};

	template <typename Key, typename Value, order_int Count, typename LessThan>
	struct TypeConverter<Elements<Key, Value, Count, LessThan>>
	{
		using From = Elements<Key, Value, Count>;
		using To = Elements<typename TypeConverter<Key>::To, typename TypeConverter<Value>::To, Count, LessThan>;

		static To ConvertFrom(From const& from, File* file)
		{
			To to(from.LessThanPtr);
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
	struct TypeConverter<MiddleNode<Key, Value, Count, StorePlace::Memory>>
	{
		using From = MiddleNode<Key, Value, Count, StorePlace::Memory>;
		using To = MiddleNode<Key, Value, Count, StorePlace::Disk>;

		static UniqueDiskPtr<NodeBase<Key, Value, Count, StorePlace::Disk>> CloneNodeToDisk(typename decltype(declval<From>()._elements)::Item const& item, File* file)
		{
			return TypeConverter<decltype(item.second)>::ConvertFrom(item.second, file);
		}

		static To ConvertFrom(From const& from, File* file)
		{
			using Node = NodeBase<Key, Value, Count, StorePlace::Disk>;
			using ::std::placeholders::_1;
			auto nodes =
				// why need decltype(from._elements.GetEnumerator()), it's obvious
				EnumeratorPipeline<typename decltype(from._elements)::Item const &, UniqueDiskPtr<Node>, decltype(from._elements.GetEnumerator())>(from._elements.GetEnumerator(), bind(&CloneNodeToDisk, _1, file));
			return { move(nodes), from._elements.LessThanPtr };
		}
	};

	template <typename Key, typename Value, order_int Count>
	struct TypeConverter<LeafNode<Key, Value, Count, StorePlace::Memory>>
	{
		using From = LeafNode<Key, Value, Count, StorePlace::Memory>;
		using To = LeafNode<Key, Value, Count, StorePlace::Disk>;

		static To ConvertFrom(From const& from, File* file)
		{
			return { TypeConverter<decltype(from._elements)>::ConvertFrom(from._elements, file), nullptr, nullptr };
		}
	};

	template <typename Key, typename Value, order_int Count>
	struct TypeConverter<NodeBase<Key, Value, Count, StorePlace::Memory>>
	{
		using From = NodeBase<Key, Value, Count, StorePlace::Memory>;
		using To = UniqueDiskPtr<NodeBase<Key, Value, Count, StorePlace::Disk>>;

		static To ConvertFrom(From const& from, File* file)
		{
			if (from.Middle())
			{
				using FromMidNode = MiddleNode<Key, Value, Count, StorePlace::Memory>;
				// 这里直接 MakeUnique 是为了让 File::New 可以直接接触到原始的类型，然后检测到继承了 TakeWithDiskPos
				// 然后 SetDiskPos
				// 如果这里变成了 shared_ptr<NodeBase>，File 接触到的 NodeBase 并没有继承 TakeWithDiskPos
				return MakeUnique(TypeConverter<FromMidNode>::ConvertFrom(static_cast<FromMidNode const&>(from), file), file);
			}
			else
			{
				using FromLeafNode = LeafNode<Key, Value, Count, StorePlace::Memory>;
				return MakeUnique(TypeConverter<FromLeafNode>::ConvertFrom(static_cast<FromLeafNode const&>(from), file), file);
			}
		}
	};

	template <typename T>
	struct TypeConverter<unique_ptr<T>>
	{
		using From = unique_ptr<T>;

		static auto ConvertFrom(From const& from, File* file)
		{
			auto p = from.get();
			auto c = TypeConverter<T>::ConvertFrom(*p, file);

			using ConvertedType = decltype(c);
			using ::Basic::IsSpecialization;

			if constexpr (IsSpecialization<ConvertedType, UniqueDiskPtr>::value)
			{
				return c;
			}
			else
			{
				return MakeUnique(move(c), file);
			}
		}

		using To = typename ReturnType<decltype(TypeConverter::ConvertFrom)>::Type;
	};

	template <typename Key, typename Value, order_int Count>
	struct TypeConverter<Btree<Count, Key, Value, StorePlace::Memory>>
	{
		using From = Btree<Count, Key, Value, StorePlace::Memory>;
		using To = Btree<Count, Key, Value, StorePlace::Disk>;

		static To ConvertFrom(From const& from, File* file)
		{
			return
			{ 
				from._keyCount, 
				TypeConverter<unique_ptr<NodeBase<Key, Value, Count, StorePlace::Memory>>>::ConvertFrom(from._root, file),
				file,
				from._lessThanPtr,
			};
		}
	};

	// 思考 Btree 中类型转换过程
	template <>
	struct TypeConverter<string, OwnerState::FullOwner>
	{
		using From = string;
		using To = UniqueDiskRef<string>;

		static To ConvertFrom(From const& from, File* file)
		{
			return { MakeUnique(from, file) };
		}
	};

	template <>
	struct TypeConverter<string, OwnerState::OwnerLess>
	{
		using From = string;
		using To = OwnerLessDiskRef<string>;

		// static To ConvertFrom(From const& from, File* file)
		// {
		// 	return { UniqueDiskPtr<string>::MakeUnique(from, file) };
		// }
	};

	template <typename... Ts>
	struct TypeConverter<tuple<Ts...>, OwnerState::FullOwner>
	{
		using From = tuple<Ts...>;
		using To = tuple<typename TypeConverter<Ts, OwnerState::FullOwner>::To...>;

		template <size_t... Idxs>
		static To ConvertEach(From const& from, index_sequence<Idxs...> idx, File* file)
		{
			return 
			{
				TypeConverter<tuple_element_t<Idxs, From>, OwnerState::FullOwner>::ConvertFrom(get<Idxs>(from), file)...
			};
		}

		static To ConvertFrom(From const& from, File* file)
		{
			return ConvertEach(from, make_index_sequence<tuple_size_v<From>>(), file);
		}
	};

	template <typename Key, typename Value>
	struct TypeConverter<pair<Key, Value>, OwnerState::FullOwner>
	{
		using From = pair<Key, Value>;
		using To = pair<typename TypeConverter<Key, OwnerState::FullOwner>::To,
						typename TypeConverter<Value, OwnerState::FullOwner>::To>;

		static To ConvertFrom(From const& from, File* file)
		{
			return pair
			{
				TypeConverter<Key, OwnerState::FullOwner>::ConvertFrom(from.first, file),
				TypeConverter<Value, OwnerState::FullOwner>::ConvertFrom(from.second, file)
			};
		}
	};

	// Disk 中对象存储的大小不稳定会影响相邻数据的存储
	template <typename T>
	struct TypeConverter<vector<T>, OwnerState::FullOwner>
	{
		using From = vector<T>;
		using SubType = vector<typename TypeConverter<T, OwnerState::FullOwner>::To>;
		using To = UniqueDiskRef<SubType>;

		static To ConvertFrom(From const& from, File* file)
		{
			auto ptr = MakeUnique(SubType(), file);

			for (auto& x : from)
			{
				ptr->push_back(TypeConverter<T, OwnerState::FullOwner>::ConvertFrom(x, file));
			}

			return To(move(ptr));
		}
	};
}