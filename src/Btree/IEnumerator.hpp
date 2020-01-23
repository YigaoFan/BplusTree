#pragma once
/**********************************
   IEnumerator interface in Collections
***********************************/

namespace Collections
{
	template <typename Item>
	class IEnumerator
	{
	public:
		virtual Item& Current() = 0;
		virtual bool MoveNext() = 0;
	};
}
