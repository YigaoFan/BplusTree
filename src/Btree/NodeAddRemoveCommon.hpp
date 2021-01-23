#pragma once
// put this file code into NodeBase static method as friend to MiddleNode and LeafNode
// Combine or change also need to set callback
#define AFTER_REMOVE_COMMON(IS_LEAF)                                                                                     \
	if (_elements.Empty())                                                                                               \
	{                                                                                                                    \
		printf("delete self");/*这里会不会是有问题的，一个节点除了在 Order 为 2 的情况下，会出现 1 外，什么情况下会只有一个节点*/                                                                                           \
		(*this->_upNodeDeleteSubNodeCallbackPtr)(this);                                                                  \
	}                                                                                                                    \
	bool nxtStealable = false, preStealable = false;                                                                     \
	if (next != nullptr)                                                                                                 \
	{                                                                                                                    \
		if (next->_elements.Count() == lowBound)                                                                         \
		{                                                                                                                \
			/* Think combine first */                                                                                    \
			if constexpr (IS_LEAF)                                                                                       \
			{                                                                                                            \
				this->SetRelationWhileCombineNext(next);                                                                 \
			}                                                                                                            \
			auto items = next->_elements.PopOutAll();                                                                    \
			(*next->_upNodeDeleteSubNodeCallbackPtr)(next); /*TODO 这里的参数有时是指针有时是硬存指针*/ \
			this->AppendItems(move(items)); /* M1 */		/*Appends*/                                                  \
			return;                                                                                                      \
		}                                                                                                                \
		else                                                                                                             \
		{                                                                                                                \
			nxtStealable = true;                                                                                         \
		}                                                                                                                \
	}                                                                                                                    \
                                                                                                                         \
	if (previous != nullptr)                                                                                             \
	{                                                                                                                    \
		if (previous->_elements.Count() == lowBound)                                                                     \
		{                                                                                                                \
			if constexpr (IS_LEAF)                                                                                       \
			{                                                                                                            \
				this->SetRelationWhileCombineToPrevious(previous); /* this code should in MiddleNode */                  \
			}                                                                                                            \
			auto items = this->_elements.PopOutAll();                                                                    \
			(*this->_upNodeDeleteSubNodeCallbackPtr)(this);                                                              \
			previous->AppendItems(move(items)); /* M2 */ /*Appends*/                                                     \
			return;                                                                                                      \
		}                                                                                                                \
		else                                                                                                             \
		{                                                                                                                \
			preStealable = true;                                                                                         \
		}                                                                                                                \
	}                                                                                                                    \
                                                                                                                         \
	if (nxtStealable && preStealable)                                                                                    \
	{                                                                                                                    \
		switch (Base1::ChooseRemovePosition(previous->_elements.Count(), this->_elements.Count(),                        \
											next->_elements.Count()))                                                    \
		{                                                                                                                \
		case Position::Next:                                                                                             \
			goto StealNxt;                                                                                               \
		case Position::Previous:                                                                                         \
			goto StealPre;                                                                                               \
		}                                                                                                                \
	}                                                                                                                    \
	else if (nxtStealable)                                                                                               \
	{                                                                                                                    \
		goto StealNxt;                                                                                                   \
	}                                                                                                                    \
	else if (preStealable)                                                                                               \
	{                                                                                                                    \
		goto StealPre;                                                                                                   \
	}                                                                                                                    \
	else                                                                                                                 \
	{                                                                                                                    \
		/* Previous and next all nullptr*/                                                                               \
		goto NoWhereToProcess;                                                                                           \
	}                                                                                                                    \
                                                                                                                         \
	StealNxt:                                                                                                            \
	{                                                                                                                    \
		auto item = next->_elements.FrontPopOut();                                                                       \
		(*next->_minKeyChangeCallbackPtr)(next->MinKey(), next);                                                         \
		this->Append(move(item)); /* M3 */ /* Append */                                                                  \
		return;                                                                                                          \
	}                                                                                                                    \
	StealPre:                                                                                                            \
	{                                                                                                                    \
		auto item = previous->_elements.PopOut();                                                                        \
		this->EmplaceHead(move(item)); /* M4 */ /*Front insert*/                                                         \
		return;                                                                                                          \
	}                                                                                                                    \
	NoWhereToProcess:

#define ADD_COMMON(IS_LEAF)                                                                                \
	bool addToPre = false, addToNxt = false;                                                               \
	if (previous == nullptr)                                                                               \
	{                                                                                                      \
		if (next != nullptr)                                                                               \
		{                                                                                                  \
			goto AddToNext;                                                                                \
		}                                                                                                  \
		else                                                                                               \
		{                                                                                                  \
			goto ConsNewNode;                                                                              \
		}                                                                                                  \
	}                                                                                                      \
	else                                                                                                   \
	{                                                                                                      \
		if (next == nullptr)                                                                               \
		{                                                                                                  \
			goto AddToPre;                                                                                 \
		}                                                                                                  \
		else                                                                                               \
		{                                                                                                  \
			switch (Base1::ChooseAddPosition(previous->_elements.Count(),                                  \
											 this->_elements.Count(),                                      \
											 next->_elements.Count()))                                     \
			{                                                                                              \
			case Position::Previous:                                                                       \
				goto AddToPre;                                                                             \
			case Position::Next:                                                                           \
				goto AddToNext;                                                                            \
			}                                                                                              \
		}                                                                                                  \
	}                                                                                                      \
                                                                                                           \
	AddToPre:                                                                                              \
	if (!previous->_elements.Full())                                                                       \
	{                                                                                                      \
		/* M5 */ previous->Append(this->ExchangeMin(move(p))); /*Append*/                                  \
		return;                                                                                            \
	}                                                                                                      \
	goto ConsNewNode;                                                                                      \
                                                                                                           \
	AddToNext:                                                                                             \
	if (!next->_elements.Full())                                                                           \
	{                                                                                                      \
		/* M6 */ next->EmplaceHead(this->ExchangeMax(move(p))); /*Front insert*/                           \
		return;                                                                                            \
	}                                                                                                      \
	goto ConsNewNode;                                                                                      \
                                                                                                           \
	ConsNewNode:                                                                                           \
	auto newNxtNode = this->NewEmptyNode(this);                                                            \
	if constexpr (IS_LEAF)                                                                                 \
	{                                                                                                      \
		this->SetRelationWhileSplitNewNext(newNxtNode.get());                                              \
	}                                                                                                      \
                                                                                                           \
	auto i = _elements.SelectBranch(p.first);                                                              \
	constexpr auto middle = BtreeOrder / 2;                                                                \
	if (i <= (middle - 1))                                                                                 \
	{                                                                                                      \
		auto items = this->_elements.PopOutItems(BtreeOrder - middle);                                     \
		/* M7 */ this->ProcessedAdd(move(p));		   /* Add (Does it duplicate to SelectBranch before)*/ \
		/* M8 */ newNxtNode->AppendItems(move(items)); /* Appends */                                       \
	}                                                                                                      \
	else                                                                                                   \
	{                                                                                                      \
		auto items = this->_elements.PopOutItems(middle);                                                  \
		/* M9 */ newNxtNode->AppendItems(move(items)); /*Appends*/                                         \
		/* M10 */ newNxtNode->ProcessedAdd(move(p));   /* Add */                                           \
	}                                                                                                      \
                                                                                                           \
	(*this->_upNodeAddSubNodeCallbackPtr)(this, move(newNxtNode));