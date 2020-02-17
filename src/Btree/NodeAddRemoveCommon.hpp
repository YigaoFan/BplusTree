#pragma once
#define AFTER_REMOVE_COMMON(IS_LEAF) \
constexpr auto lowBound = Base::LowBound;\
if (_elements.Count() < lowBound)\
{\
	if (_elements.Empty())\
	{\
		this->_upNodeDeleteSubNodeCallback(this);\
	}\
	bool nxtStealable = false, preStealable = false;\
	if (_next != nullptr)\
	{\
		if (_next->_elements.Count() == lowBound)\
		{\
			/* Think combine first */\
			auto items = _next->_elements.PopOutItems(_next->_elements.Count());\
			this->_elements.Add(move(items));/*Appends*/\
			if constexpr (IS_LEAF)\
			{\
				this->_next = _next->_next;\
				_next->_next->_previous = this;\
			}\
			_next->_upNodeDeleteSubNodeCallback(_next);\
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
			_previous->_elements.Add(move(items));/*Appends*/\
			if constexpr (IS_LEAF)\
			{\
				_previous->_next = _next;\
				_next->_previous = _previous;\
			}\
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
		/* previous and next all nullptr*/\
		goto NoWhereToProcess;\
	}\
\
StealNxt:\
	{\
		auto item = _next->_elements.FrontPopOut();\
		_next->_minKeyChangeCallback(_next->MinKey(), _next);\
		this->_elements.Append(move(item));\
		return;\
	}\
StealPre:\
	{\
		auto items = _previous->_elements.PopOutItems(1);\
		this->_elements.Insert(move(items[0]));/*Front insert*/\
		this->_minKeyChangeCallback(this->MinKey(), this);\
		return;\
	}\
}\
NoWhereToProcess:\
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
	_previous->_elements.Append(_elements.ExchangeMin(move(p)));\
	this->_minKeyChangeCallback(this->MinKey(), this);\
	return;\
}\
goto ConsNewNode;\
AddToNext:\
if (!_next->_elements.Full())\
{\
	_next->_elements.Insert(this->_elements.ExchangeMax(move(p)));/*Front insert*/\
	_next->_minKeyChangeCallback(_next->MinKey(), _next);\
	return;\
}\
goto ConsNewNode;\
\
ConsNewNode:\
auto newNxtNode = make_unique<remove_pointer_t<decltype(this)>>(_elements.LessThanPtr);\
if constexpr (IS_LEAF)\
{\
	newNxtNode->_next = this->_next;\
	newNxtNode->_previous = this;\
	this->_next = newNxtNode.get();\
}\
\
auto i = _elements.SelectBranch(p.first);\
constexpr auto middle = (BtreeOrder % 2) ? (BtreeOrder / 2) : (BtreeOrder / 2 + 1);\
if (i <= middle)\
{\
	auto items = this->_elements.PopOutItems(middle);\
	this->_elements.Add(move(p));\
	if (i == 0)\
	{\
		this->_minKeyChangeCallback(this->MinKey(), this);\
	}\
	newNxtNode->_elements.Add(move(items));\
}\
else\
{\
	auto items = this->_elements.PopOutItems(BtreeOrder - middle);\
	newNxtNode->_elements.Add(move(items));/*Appends*/\
	newNxtNode->_elements.Add(move(p));\
}\
\
this->_upNodeAddSubNodeCallback(this, move(newNxtNode));