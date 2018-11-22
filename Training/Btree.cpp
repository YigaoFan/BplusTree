#include "pch.h"
#include "Btree.h"


Btree::Btree()
{
}


Btree::~Btree()
{
}

std::ostream& operator<<(std::ostream& os, const Btree& item)
{
	os << item.root.childCount << std::endl;
	return os;
}