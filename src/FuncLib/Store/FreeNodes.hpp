#pragma once
#include <optional>
#include "LableRelationNode.hpp"
#include "LableTree.hpp"
#include "VectorUtil.hpp"

namespace FuncLib::Store
{
	using ::std::back_inserter;
	using ::std::optional;

	class FreeNodes : private LableTree
	{
	public:
		// optional<LableRelationNode> Take(pos_lable lable)
		// {
		// 	for (size_t i = 0; auto& n : _nodes)
		// 	{
		// 		if (n.Lable() == lable)
		// 		{
		// 			auto des = move(n);
		// 			Erase(i, _nodes);
		// 			return { move(des) };
		// 		}
		// 		else if (optional<LableRelationNode> r = n.TakeInternal(lable); r.has_value())
		// 		{
		// 			return r;
		// 		}

		// 		++i;
		// 	}

		// 	return {};
		// }

		// 下面这两个函数酌情看哪个不用就删掉 TODO
		// void Add(LableRelationNode notUsedNode)
		// {
		// 	_fileRoot.AddSub(move(notUsedNode));
		// }

		void Add(vector<LableRelationNode> notUsedNodes)
		{
			for (auto& n : notUsedNodes)
			{
				this->AddSub(move(n));
			}
		}

		template <typename Releaser>
		void ReleaseAll(Releaser const& releaser)
		{
			// 是不是改成 Release Subs
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
