#include <iostream>
#include "MemoryMemo.hpp"

int MemoryMemo::Count = 0;

MemoryMemo::MemoryMemo()
	: _value(MemoryMemo::Count++)
{ }

MemoryMemo::MemoryMemo(MemoryMemo&& that)
	: _value(that._value)
{
	that._hasValue = false;
}

MemoryMemo::MemoryMemo(MemoryMemo const& that)
	: MemoryMemo()
{ }

MemoryMemo& MemoryMemo::operator= (MemoryMemo const& that)
{
	this->_value = MemoryMemo::Count++;
	return *this;
}

MemoryMemo& MemoryMemo::operator= (MemoryMemo&& that)
{
	this->_value = that._value;
	that._hasValue = false;
	return *this;
}

MemoryMemo::~MemoryMemo()
{
	using ::std::cout;
	using ::std::endl;

	if (_hasValue)
	{
		cout << _value << " Destroyed" << endl;
	}
}