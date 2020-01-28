#pragma once
#define REMOVE_COMMON  constexpr auto lowBound = Base::LowBound;\
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