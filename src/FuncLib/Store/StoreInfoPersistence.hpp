#pragma once
#include "ObjectRelation/ObjectRelationTree.hpp"
#include "../Persistence/ByteConverter.hpp"
#include "../../Btree/Generator.hpp"

/// 其他类支持 ByteConverter 的样板
namespace FuncLib::Persistence
{
	using FuncLib::Store::StorageAllocator;

	template <>
	struct ByteConverter<StorageAllocator, false>
	{
		using ThisType = StorageAllocator;
		using DataMember0 = decltype(declval<ThisType>()._currentPos);
		using DataMember1 = decltype(declval<ThisType>()._usingLabelTable);
		using DataMember2 = decltype(declval<ThisType>()._deletedLabelTable);
		static constexpr bool SizeStable = All<GetSizeStable, DataMember0, DataMember1, DataMember2>::Result;

		static void WriteDown(ThisType const &p, IWriter auto *writer)
		{
			ByteConverter<DataMember0>::WriteDown(p._currentPos, writer);
			ByteConverter<DataMember1>::WriteDown(p._usingLabelTable, writer);
			ByteConverter<DataMember2>::WriteDown(p._deletedLabelTable, writer);
		}

		static ThisType ReadOut(FileReader *reader)
		{
			return {
				ByteConverter<DataMember0>::ReadOut(reader),
				ByteConverter<DataMember1>::ReadOut(reader),
				ByteConverter<DataMember2>::ReadOut(reader)};
		}
	};
}

namespace FuncLib::Store
{
	using namespace Persistence;
	using Collections::Generator;
	using ObjectRelation::ObjectRelationTree;

	Generator<optional<vector<pos_label>>> ReadSubNodesLabels(IReader auto *reader)
	{
		for (vector<pos_label> labels;;)
		{
			auto label = ByteConverter<pos_label>::ReadOut(reader);
			if (NonLabel == label)
			{
				if (labels.empty())
				{
					co_yield {};
				}
				else
				{
					co_yield move(labels);
				}
			}
			else
			{
				labels.push_back(label);
			}
		}
	}

	vector<LabelNode> ReadOutSubNodes(Generator<optional<vector<pos_label>>> *generator);

	// 下面这个函数有模板，所以很难声明和定义分离，如果放在 ObjectRelationTree.hpp 里就会影响 ObjectRelationTree 的使用
	// 因为 File 包含了 ObjectRelationTree，File 不能看到 ByteConverter，所以 ObjectRelationTree.hpp 里不能放 ByteConverter
	// 但如果想单独在 cpp 文件里测试 ReadObjRelationTreeFrom，就要添加 include ByteConverter，这样就不好，使用者不一定需要知道要用
	// ByteConverter，所以用 friend 函数将这个函数单独拎出来，拥有更大的自由度，需要使用包含这个就可以了。各部分的功能更专一。
	/// Not write free nodes
	ObjectRelationTree ReadObjRelationTreeFrom(IReader auto* reader)
	{
		auto fileLabel = ByteConverter<pos_label>::ReadOut(reader);
		auto gen = ReadSubNodesLabels(reader);
		auto root = LabelNode(fileLabel, ReadOutSubNodes(&gen));
		return ObjectRelationTree(move(root));
	}

	void WriteObjRelationTree(ObjectRelationTree const& tree, ObjectBytes* writer);

	StorageAllocator ReadAllocatedInfoFrom(IReader auto* reader)
	{
		return ByteConverter<StorageAllocator>::ReadOut(reader);
	}

	void WriteAllocatedInfoTo(StorageAllocator const &allocator, ObjectBytes *bytes);
}