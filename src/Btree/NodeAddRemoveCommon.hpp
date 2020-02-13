#pragma once
#define REMOVE_COMMON \
constexpr auto lowBound = Base::LowBound;\
if (_elements.Count() < lowBound)\
{\
	bool nxtStealable = false, preStealable = false;\
	if (_next != nullptr)\
	{\
		if (_next->_elements.Count() == lowBound)\
		{\
			auto items = this->_elements.PopOutItems(this->_elements.Count());\
			_next->_elements.Add(move(items));\
			this->_upNodeDeleteSubNodeCallback(this);\
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
			auto items = this->_elements.PopOutItems(this->_elements.Count());\
			_previous->_elements.Add(move(items));\
			this->_upNodeDeleteSubNodeCallback(this);\
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
		goto NothingToDo;\
	}\
\
StealNxt:\
	{\
		auto item = _next->_elements.FrontPopOut();\
		this->_elements.Append(move(item));\
		return;\
	}\
StealPre:\
	{\
		auto items = _previous->_elements.PopOutItems(1);\
		this->_elements.Insert(move(items[0]));\
		return;\
	}\
NothingToDo:\
	return;\
}

#define ADD_COMMON(IS_LEAF) \
bool addToPre = false, addToNxt = false;\
if (_previous == nullptr)\
{\
	if (_next != nullptr)\
	{\
		addToNxt = true;\
	}\
}\
else\
{\
	if (_next == nullptr)\
	{\
		addToPre = true;\
	}\
	else\
	{\
		switch (Base::ChooseOperatePosition<Operation::Add>(_previous->_elements.Count(),\
			this->_elements.Count(),\
			_next->_elements.Count()))\
		{\
		case Position::Previous:\
			addToPre = true;\
			break;\
		case Position::Next:\
			addToNxt = true;\
			break;\
		}\
	}\
}\
\
if (addToPre)\
{\
	if (!_previous->_elements.Full())\
	{\
		_previous->_elements.Append(_elements.ExchangeMin(move(p)));\
		return;\
	}\
}\
else if (addToNxt)\
{\
	if (!_next->_elements.Full())\
	{\
		_next->_elements.Insert(_elements.ExchangeMax(move(p)));\
		return;\
	}\
}\
\
auto newNxtNode = make_unique<remove_pointer_t<decltype(this)>>(_elements.LessThanPtr);\
if constexpr (IS_LEAF)\
{\
newNxtNode->_next = this->_next;\
newNxtNode->_previous = this;\
this->_next = newNxtNode.get();\
}\
\
auto i = _elements.SelectBranch(p.first);\
constexpr auto middle = (BtreeOrder % 2) ? (BtreeOrder / 2 + 1) : (BtreeOrder / 2);\
if (i <= middle)\
{\
	auto items = this->_elements.PopOutItems(middle);\
	this->_elements.Add(move(p));\
	newNxtNode->_elements.Add(move(items));\
}\
else\
{\
	auto items = this->_elements.PopOutItems(BtreeOrder - middle);\
	newNxtNode->_elements.Add(move(items));\
	newNxtNode->_elements.Add(move(p));\
}\
\
this->_upNodeAddSubNodeCallback(this, move(newNxtNode));