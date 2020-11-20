#pragma once
#include "LabelTree.hpp"

namespace FuncLib::Store
{
	class FreeNodes : private LabelTree
	{
	public:
		using LabelTree::AddSub;
		using LabelTree::Take;

		template <typename Releaser>
		void ReleaseAll(Releaser const& releaser)
		{
			_fileRoot.ReleaseAll(releaser);
		}
		
		// FreeNodes(FreeNodes const& that) = delete;
		// FreeNodes(FreeNodes&& that) noexcept = default;
		// 有了下面这个函数，加上上面不声明那两个 default 和 delete，就会报使用了 vector 的移动构造函数的错（Node 不能复制构造）
		// 可能是下面这个定义了就触发了编译器合成复制构造函数了？
		// ~FreeNodes()
		// {

		// }
	};	
}
