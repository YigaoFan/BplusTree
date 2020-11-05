#include "ObjectBytesQueue.hpp"

namespace FuncLib::Store
{
	ObjectBytesQueue::ObjectBytesQueue(vector<ObjectBytes *> objBytesQueue) : _objBytesQueue(move(objBytesQueue))
	{ }

	void ObjectBytesQueue::Add(ObjectBytes* objectBytes)
	{
		_objBytesQueue.push_back(objectBytes);
	}
}
