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
		pos_label _label;
		vector<FakeWriter> _subWriters;

	public:
		FakeWriter(pos_label label);
		FakeWriter(FakeWriter const& that) = delete;
		FakeWriter(FakeWriter&& that) noexcept = default;

		void Add(char const* begin, size_t size);
		void AddBlank(size_t size);
		void AddSub(FakeWriter subWriter);
		pos_label Label() const;
		auto GetLabelSortedSubsEnumerator() const { return CreateEnumerator(_subWriters); }
	};
}
