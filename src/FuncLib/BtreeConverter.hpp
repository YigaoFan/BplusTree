#pragma once
#include <memory>
#include "DiskPtr.hpp"
#include "DiskDataConverter.hpp"
#include "File.hpp"
#include "../Btree/Basic.hpp"
#include "../Btree/Btree.hpp"
#include "../Btree/NodeBase.hpp"
#include "../Btree/LeafNode.hpp"
#include "../Btree/MiddleNode.hpp"

namespace FuncLib
{
	using ::std::shared_ptr;
	using ::std::unique_ptr;
	using ::Collections::order_int;
	using ::Collections::LessThan;
	using ::Collections::Btree;

	template <typename Key, typename Value, order_int Order>
	class BtreeConverter
	{
	private:
		using Content = Btree<Order, Key, Value, DiskPtr>;
	public:
		static DiskPtr<Content> ConvertToDisk(Btree<Order, Key, Value, unique_ptr> const& btree, shared_ptr<File> file)
		{
			using T = Btree<Order, Key, Value, unique_ptr>;
			return file->Allocate<T>(DiskDataConverter<T>::ConvertToDiskData(btree, file));
		}

		static DiskPtr<Content>
		ReadTreeFrom(shared_ptr<File> file, shared_ptr<LessThan<Key>> lessThanPtr, size_t startOffset = 0)
		{
			auto p = DiskPtr<Content>({ file, startOffset });
			p.RegisterSetter([lessThanPtr = move(lessThanPtr)](Content* treePtr)
			{
				treePtr->_lessThanPtr = lessThanPtr;
				SetProperty(treePtr->_root, [lessThanPtr = lessThanPtr](typename Content::Node* node)
				{
					node->_elements.LessThanPtr = lessThanPtr;
				});
			});
			return p;
		}
	};
}