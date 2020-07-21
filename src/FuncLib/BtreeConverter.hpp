#pragma once
#include <memory>
#include "ByteConverter.hpp"
#include "TypeConverter.hpp"
#include "File.hpp"
#include "../Btree/Basic.hpp"
#include "../Btree/Btree.hpp"
#include "../Btree/NodeBase.hpp"
#include "../Btree/LeafNode.hpp"
#include "../Btree/MiddleNode.hpp"

namespace FuncLib
{
	using ::std::shared_ptr;
	using ::std::make_shared;
	using ::std::unique_ptr;
	using ::Collections::order_int;
	using ::Collections::LessThan;
	using ::Collections::Btree;

	template <typename Key, typename Value, order_int Order>
	class BtreeConverter
	{
	private:
		using Converted = Btree<Order, Key, Value, UniqueDiskPtr>;

	public:
		static UniqueDiskPtr<Converted>
		ConvertToDisk(Btree<Order, Key, Value, unique_ptr> const& btree, shared_ptr<File> file)
		{
			using T = Btree<Order, Key, Value, unique_ptr>;
			return UniqueDiskPtr<Converted>::MakeUnique(make_shared<Converted>(TypeConverter<T>::ConvertFrom(btree, file)), file);
		}

		static UniqueDiskPtr<Converted>
		ReadTreeFrom(shared_ptr<File> file, shared_ptr<LessThan<Key>> lessThanPtr, size_t startOffset = 0)
		{
			auto p = UniqueDiskPtr<Converted>({ file, startOffset });
			p.RegisterSetter([lessThanPtr = move(lessThanPtr)](Converted* treePtr)
			{
				treePtr->LessThanPredicate(lessThanPtr);
			});

			return p;
		}
	};
}