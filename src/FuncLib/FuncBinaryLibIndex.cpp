#include <cassert>
#include "FuncBinaryLibIndex.hpp"

namespace FuncLib
{
	using ::std::move;
	using ::std::filesystem::exists;

	FuncBinaryLibIndex::FuncBinaryLibIndex(shared_ptr<File> file, shared_ptr<DiskBtree> diskBtree)
		: _file(move(file)), _diskBtree(move(diskBtree))
	{ }

	FuncBinaryLibIndex FuncBinaryLibIndex::GetFrom(path const &path)
	{
		auto firstSetup = not exists(path);
		auto file = File::GetFile(path);

		shared_ptr<DiskBtree> tree;
		auto pred = [](string const &s1, string const &s2) {
			return s1 < s2;
		};

		if (firstSetup)
		{
			auto [l, tree] = file->New(DiskBtree(move(pred)));
			assert(l == 1); // l should be 1
		}
		else
		{
			constexpr pos_label btreeLabel = 1; // 这里是 hardcode 为 1
			tree = file->Read<DiskBtree>(btreeLabel);
			tree->LessThanPredicate(move(pred));
		}

		return FuncBinaryLibIndex(move(file), move(tree));
	}

#define STR_TO_DISK_REF_STR(STR) TypeConverter<string, OwnerState::FullOwner>::ConvertFrom(STR, _file.get())
	void FuncBinaryLibIndex::Add(FuncType const& type, pair<pos_label, string> info)
	{
		_diskBtree->Add({ STR_TO_DISK_REF_STR(type.ToKey()), { info.first, STR_TO_DISK_REF_STR(info.second) } });
	}

	pos_label FuncBinaryLibIndex::GetStoreLabel(FuncType const& type) const
	{
		return _diskBtree->GetValue(type.ToKey()).first;
	}

	bool FuncBinaryLibIndex::Contains(FuncType const& type) const
	{
		return _diskBtree->ContainsKey(type.ToKey());
	}

	void FuncBinaryLibIndex::ModifyFuncName(FuncType type, string newFuncName)
	{
		ModifyType(move(type), [n = move(newFuncName)](FuncType oldType)
		{
			oldType.FuncName(move(n));
			return oldType;
		});
	}

	void FuncBinaryLibIndex::ModifyPackageOf(FuncType type, vector<string> packageHierarchy)
	{
		ModifyType(move(type), [h = move(packageHierarchy)](FuncType oldType)
		{
			oldType.PackageHierarchy(move(h));
			return oldType;
		});
	}

	void FuncBinaryLibIndex::Remove(FuncType const& type)
	{
		_diskBtree->Remove(type.ToKey());
	}

	auto FuncBinaryLibIndex::Search(string const& keyword) -> Generator<pair<Key, pair<pos_label, string>>>
	{
		auto g = _diskBtree->GetStoredPairEnumerator();
		auto includedIn = [&keyword](string const& word)
		{
			return word.find(keyword) != string::npos;
		};

		while (g.MoveNext())
		{
			auto p = g.Current();
			if (includedIn(p->first))
			{
				// 后续如果 FuncType::ToKey 的形成规则变了，这里也要变
				co_yield { Key(p->first), { p->second.first, Key(p->second.second) } };
			}
			else if (includedIn(p->second.second))
			{
				co_yield { Key(p->first), { p->second.first, Key(p->second.second) } };
			}
		}
	}

	template <typename Callback>
	void FuncBinaryLibIndex::ModifyType(FuncType oldType, Callback genNewTypeCallback)
	{
		_diskBtree->ModifyKey(oldType.ToKey(), STR_TO_DISK_REF_STR(genNewTypeCallback(oldType).ToKey()));
	}
#undef STR_TO_DISK_REF_STR
}
