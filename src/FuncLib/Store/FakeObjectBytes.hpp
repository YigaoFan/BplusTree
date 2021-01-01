#pragma once
#include "StaticConfig.hpp"
#include "LabelNodeBase.hpp"

namespace FuncLib::Store
{
	class FakeObjectBytes : private LabelNodeBase<FakeObjectBytes>
	{
	private:
		using Base = LabelNodeBase<FakeObjectBytes>;

	public:
		FakeObjectBytes(pos_label label);

		void Add(char const* begin, size_t size);
		void AddBlank(size_t size);

		using Base::AddSub;
		using Base::CreateSortedSubNodeEnumerator;
		using Base::Label;
	};
}
