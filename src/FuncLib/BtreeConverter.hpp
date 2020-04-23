#pragma once
#include <memory>
#include "DiskPtr.hpp"
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
		using Entity = Btree<Order, Key, Value, DiskPtr>;

	public:
		static DiskPtr<Entity>
		ConvertToDisk(Btree<Order, Key, Value, unique_ptr> const& btree, shared_ptr<File> file)
		{
			using T = Btree<Order, Key, Value, unique_ptr>;
			return DiskPtr<Entity>::MakeDiskPtr(make_shared<Entity>(TypeConverter<T>::ConvertFrom(btree, file)), file);
		}

		// file 里面的分配信息是不是要保存一下，只是读倒没有问题，写就有问题了
		static DiskPtr<Entity>
		ReadTreeFrom(shared_ptr<File> file, shared_ptr<LessThan<Key>> lessThanPtr, size_t startOffset = 0)
		{
			auto p = DiskPtr<Entity>({ file, startOffset });
			p.RegisterSetter([lessThanPtr = move(lessThanPtr)](Entity* treePtr)
			{
				treePtr->_lessThanPtr = lessThanPtr;
				SetProperty(treePtr->_root, [lessThanPtr = lessThanPtr](typename Entity::Node* node)
				{
					node->_elements.LessThanPtr = lessThanPtr;
				});
			});
			// 这里还有个关键点是提供那几个回调
			return p;
		}
	};
}