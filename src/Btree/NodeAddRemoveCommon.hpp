#pragma once
// Combine or change also need to set callback
#define AFTER_REMOVE_COMMON(IS_LEAF) \
constexpr auto lowBound = Base::LowBound;\
if (_elements.Count() < lowBound)\
{\
	if (_elements.Empty())\
	{\
		(*this->_upNodeDeleteSubNodeCallbackPtr)(this);\
	}\
	bool nxtStealable = false, preStealable = false;\
	if (_next != nullptr)\
	{\
		if (_next->_elements.Count() == lowBound)\
		{\
			/* Think combine first */\
			auto items = _next->_elements.PopOutAll();\
			this->AppendItems(move(items));/* M1 *//*Appends*/\
			if constexpr (IS_LEAF)\
			{\
				this->SetRelationWhileCombineNext(_next);\
			}\
			(*_next->_upNodeDeleteSubNodeCallbackPtr)(_next);\
			return;\
		}\
		else\
		{\
			nxtStealable = true;\
		}\
	}\
\
	if (_previous != nullptr)\
	{\
		if (_previous->_elements.Count() == lowBound)\
		{\
			auto items = this->_elements.PopOutAll();\
			_previous->AppendItems(move(items));/* M2 *//*Appends*/\
			if constexpr (IS_LEAF)\
			{\
				this->SetRelationWhileCombineToPrevious(_previous);\
			}\
			(*this->_upNodeDeleteSubNodeCallbackPtr)(this);\
			return;\
		}\
		else\
		{\
			preStealable = true;\
		}\
	}\
\
	if (nxtStealable && preStealable)\
	{\
		switch (Base::ChooseOperatePosition<Operation::Remove>(_previous->_elements.Count(), this->_elements.Count(),\
			_next->_elements.Count()))\
		{\
		case Position::Next:\
			goto StealNxt;\
		case Position::Previous:\
			goto StealPre;\
		}\
	}\
	else if (nxtStealable)\
	{\
		goto StealNxt;\
	}\
	else if (preStealable)\
	{\
		goto StealPre;\
	}\
	else\
	{\
		/* Previous and next all nullptr*/\
		goto NoWhereToProcess;\
	}\
\
StealNxt:\
	{\
		auto item = _next->_elements.FrontPopOut();\
		(*_next->_minKeyChangeCallbackPtr)(_next->MinKey(), _next);\
		this->Append(move(item));/* M3 *//* Append */\
		return;\
	}\
StealPre:\
	{\
		auto item = _previous->_elements.PopOut();\
		this->EmplaceHead(move(item));/* M4 *//*Front insert*/\
		return;\
	}\
}\
else\
{\
	return;\
}\
NoWhereToProcess:
// Remove should think of set of _previous and _next of LeafNode

#define ADD_COMMON(IS_LEAF) \
bool addToPre = false, addToNxt = false;\
if (_previous == nullptr)\
{\
	if (_next != nullptr)\
	{\
		goto AddToNext;\
	}\
	else\
	{\
		goto ConsNewNode;\
	}\
}\
else\
{\
	if (_next == nullptr)\
	{\
		goto AddToPre;\
	}\
	else\
	{\
		switch (Base::ChooseOperatePosition<Operation::Add>(_previous->_elements.Count(),\
			this->_elements.Count(),\
			_next->_elements.Count()))\
		{\
		case Position::Previous:\
			goto AddToPre;\
		case Position::Next:\
			goto AddToNext;\
		}\
	}\
}\
\
AddToPre:\
if (!_previous->_elements.Full())\
{\
	/* M5 */_previous->Append(this->ExchangeMin(move(p)));/*Append*/\
	return;\
}\
goto ConsNewNode;\
AddToNext:\
if (!_next->_elements.Full())\
{\
	/* M6 */_next->EmplaceHead(this->ExchangeMax(move(p)));/*Front insert*/\
	return;\
}\
goto ConsNewNode;\
\
ConsNewNode:\
auto newNxtNode = make_unique<remove_pointer_t<decltype(this)>>(_elements.LessThanPtr);\
if constexpr (IS_LEAF)\
{\
	newNxtNode->_next = this->_next;/* TODO _previous and _next maybe not exist */\
	newNxtNode->_previous = this;\
	this->_next = newNxtNode.get();\
}\
\
auto i = _elements.SelectBranch(p.first);\
constexpr auto middle = (BtreeOrder % 2) ? (BtreeOrder / 2) : (BtreeOrder / 2 + 1);\
if (i <= middle)\
{\
	auto items = this->_elements.PopOutItems(middle);\
	/* M7 */this->ProcessedAdd(move(p));/* Add (Does it duplicate to SelectBranch before)*/\
	/* M8 */newNxtNode->AppendItems(move(items));/* Appends */\
}\
else\
{\
	auto items = this->_elements.PopOutItems(BtreeOrder - middle);\
	/* M9 */newNxtNode->AppendItems(move(items));/*Appends*/\
	/* M10 */newNxtNode->ProcessedAdd(move(p));/* Add */\
}\
\
(*this->_upNodeAddSubNodeCallbackPtr)(this, move(newNxtNode));