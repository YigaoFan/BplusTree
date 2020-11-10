#pragma once
#include <vector>
#include "StaticConfig.hpp"
#include "../../Btree/Enumerator.hpp"

namespace FuncLib::Store
{
	using Collections::CreateEnumerator;
	using ::std::vector;

	class FakeWriter
	{
	private:
		pos_lable _lable;
		vector<FakeWriter> _subWriters;

	public:
		FakeWriter(pos_lable lable);
		FakeWriter(FakeWriter const& that) = delete;
		FakeWriter(FakeWriter&& that) noexcept = default;

		void Add(char const* begin, size_t size);
		void AddBlank(size_t size);
		void AddSub(FakeWriter subWriter);
		pos_lable Lable() const;
		auto GetLableSortedSubsEnumerator() const { return CreateEnumerator(_subWriters); }
	};
}
