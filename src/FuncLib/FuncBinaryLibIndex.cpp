#include "FuncBinaryLibIndex.hpp"

namespace FuncLib
{
	using ::std::move;
	using ::std::filesystem::exists;

	constexpr pos_label DiskTreeLable = 100;

	FuncBinaryLibIndex::FuncBinaryLibIndex(shared_ptr<File> file, shared_ptr<DiskBtree> diskBtree)
		: _file(move(file)), _diskBtree(move(diskBtree))
	{ }

	FuncBinaryLibIndex::~FuncBinaryLibIndex()
	{
		if (_file != nullptr and _diskBtree != nullptr)
		{
			_file->Store(DiskTreeLable, _diskBtree);
		}
	}

	FuncBinaryLibIndex FuncBinaryLibIndex::GetFrom(path const &path)
	{
		auto firstSetup = not exists(path);
		auto file = File::GetFile(path);

		shared_ptr<DiskBtree> tree;

		if (firstSetup)
		{
			auto [l, t] = file->New(DiskTreeLable, DiskBtree(file.get()));
			tree = move(t);
		}
		else
		{
			tree = file->Read<DiskBtree>(DiskTreeLable);
		}

		return FuncBinaryLibIndex(move(file), move(tree));
	}

#define STR_TO_DISK_REF_STR(STR) TypeConverter<string, OwnerState::FullOwner>::ConvertFrom(STR, _file.get())
	void FuncBinaryLibIndex::Add(FuncObj const& funcObj, pos_label label)
	{
		auto joinWithSpace = [](vector<string> const& strs)
		{
			string combined;
			for (auto& s : strs)
			{
				combined.append(s + ' ');
			}

			if (not strs.empty())
			{
				combined.pop_back();
			}

			return combined;
		};

		_diskBtree->Add(pair(STR_TO_DISK_REF_STR(funcObj.Type.ToKey()),
			pair(label,
				pair(STR_TO_DISK_REF_STR(funcObj.Summary),
					STR_TO_DISK_REF_STR(joinWithSpace(funcObj.ParaNames))
				))));
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
			oldType.FuncName = move(n);
			return oldType;
		});
	}

	void FuncBinaryLibIndex::ModifyPackageOf(FuncType type, vector<string> packageHierarchy)
	{
		ModifyType(move(type), [h = move(packageHierarchy)](FuncType oldType)
		{
			oldType.PackageHierarchy = move(h);
			return oldType;
		});
	}

	void FuncBinaryLibIndex::Remove(FuncType const& type)
	{
		_diskBtree->Remove(type.ToKey());
	}

	auto FuncBinaryLibIndex::Search(string const& keyword) const -> Generator<pair<string, string>>
	{
		auto g = _diskBtree->GetStoredPairEnumerator();
		auto includedIn = [&keyword](string const& word)
		{
			return word.find(keyword) != string::npos;
		};

		while (g.MoveNext())
		{
			auto p = g.Current();
			if (includedIn(p->first)) // Key
			{
				// 后续如果 FuncType::ToKey 的形成规则变了，这里也要变
#define YIELD_EXP co_yield { string(p->first), string(p->second.second.first) }
				YIELD_EXP;
			}
			else if (includedIn(p->second.second.first)) // summary
			{
				YIELD_EXP;
#undef YIELD_EXP
			}
		}
	}

	template <typename Callback>
	void FuncBinaryLibIndex::ModifyType(FuncType oldType, Callback genNewTypeCallback)
	{
		_diskBtree->ModifyKey(oldType.ToKey(), STR_TO_DISK_REF_STR(genNewTypeCallback(oldType).ToKey()));
	}
#undef STR_TO_DISK_REF_STR

	Generator<FuncType> FuncBinaryLibIndex::FuncTypes() const
	{
		auto g = _diskBtree->GetStoredPairEnumerator();
		while (g.MoveNext())
		{
			auto p = g.Current();
			string k = p->first;
			co_yield FuncType::FromKey(k);
		}
	}
}
