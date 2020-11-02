#pragma once
#include <vector>
#include <iterator>
#include <functional>
#include "StaticConfig.hpp"

#include "../../Basic/Concepts.hpp" // temp

namespace FuncLib::Store
{
	using ::std::back_inserter;
	using ::std::function;
	using ::std::move;
	using ::std::vector;



	using Basic::IsSameTo;
	template <typename T>
	concept PosLableNode = requires(T node)
	{
		{ node.Lable() } -> IsSameTo<pos_lable>;
		{ node.Next() } -> PosLableNode;
		{ node.SubNodes() } -> PosLableNode;
	};

	// 每一个 writer 里面配一个 LableRelationNode*
	class LableRelationNode
	{
	private:
		function<void(pos_lable)> _releaser;
		pos_lable _lable;
		vector<LableRelationNode*> _subLables;// 用 shared_ptr TODO
	public:
		LableRelationNode(pos_lable lable, function<void(pos_lable)> releaser);

		LableRelationNode(LableRelationNode&& that) noexcept = delete;
		LableRelationNode(LableRelationNode const& that) = delete;

		template <typename ReadQuerier>
		void UpdateWith(PosLableNode auto* node, ReadQuerier const& read)
		{
			if (not read(_lable))
			{
				return;
			}

			if (auto l = node->Lable(); l != this->_lable)
			{
				this->ReleaseAll();
				this->_lable = l;
				this->_subLables = move(newInfoRealtionNode._subLables);// traverse then cons tree
			}
			else
			{
				for (int i = _subLables.size() - 1; i >= 0; --i)
				{
					auto oldP = this->_subLables[i];
					auto oldLable = oldP->_lable;
					
					if (not read(oldLable))
					{
						continue; // 如果这个 lable 没读，那就跳过下面这些步骤
					}

					auto subs = node->SubNodes();// Enumerator like
					while (subs.MoveNext())
					{
						PosLableNode* subPtr = subs.Current();
						auto newLable = subPtr->Lable();

						if (oldLable == newLable)
						{
							oldP->UpdateWith(subPtr, read);
							goto NextOld;
						}
					}

					// for (int j = newInfoRealtionNode._subLables.size() - 1; j >= 0; --j)
					// {
					// 	// auto newP = newInfoRealtionNode._subLables[j];
					// 	auto newLable = subPtr->Lable();

					// 	if (oldLable == newLable)
					// 	{
					// 		oldP->UpdateWith(subPtr, read);
					// 		// delete newP;
					// 		// newInfoRealtionNode._subLables.erase(newInfoRealtionNode._subLables.begin() + j);
					// 		goto NextOld;
					// 	}
					// }
				Release:
					oldP->ReleaseAll();
					delete oldP;
					_subLables.erase(_subLables.begin() + i);
				NextOld:
					break;
				}

				// need to collect new pos_lable subs
				// traverse then cons tree
				// include new node
				move(newInfoRealtionNode._subLables.begin(), newInfoRealtionNode._subLables.end(), back_inserter(this->_subLables));
			}
		}

		LableRelationNode* AddSub(pos_lable lable);
	
	private:
		void ReleaseAll();
		~LableRelationNode();
	};	
}
