#include "ObjectBytesQueue.hpp"

namespace FuncLib::Store
{
	using ::std::move;
	
	ObjectBytesQueue::ObjectBytesQueue(vector<ObjectBytes> objBytesQueue) : _objBytesQueue(move(objBytesQueue))
	{ }

	void ObjectBytesQueue::Add(ObjectBytes objectBytes)
	{
		_objBytesQueue.push_back(move(objectBytes));
	}
}
