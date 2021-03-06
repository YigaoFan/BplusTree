#pragma once
#include "StaticConfig.hpp"
#include "ObjectRelation/LabelNodeBase.hpp"

namespace FuncLib::Store
{
	using namespace FuncLib::Store::ObjectRelation;

	class FakeObjectBytes : private LabelNodeBase<FakeObjectBytes>
	{
	private:
		using Base = LabelNodeBase<FakeObjectBytes>;
		bool _written = false;

	public:
		using Base::Base;
		bool Written() const;
		void Add(char const* begin, size_t size);
		void AddBlank(size_t size);
		using Base::ConstructSub;
		using Base::CreateSortedSubNodeEnumerator;
		using Base::Label;
	};
}
