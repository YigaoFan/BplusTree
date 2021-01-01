#pragma once
#include "../../Basic/Concepts.hpp"
#include "StaticConfig.hpp"

namespace FuncLib::Store
{
	using Basic::IsSameTo;

	template <typename T>
	concept Enumerator = requires(T e)
	{
		e.MoveNext();
		e.Current();
	};

	template <typename T>
	concept PosLabelNode = requires(T node)
	{
		{ node.Label() } -> IsSameTo<pos_label>;
		{ node.CreateSortedSubNodeEnumerator() } -> Enumerator;
	};	
}
