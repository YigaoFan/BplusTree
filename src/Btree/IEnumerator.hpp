#pragma once
/***********************************************************************************************************
   IEnumerator interface in Collections
***********************************************************************************************************/

namespace Collections
{
	using ::std::size_t;

	template <typename Item>
	class IEnumerator
	{
	public:
		virtual Item& Current() = 0;
		virtual bool MoveNext() = 0;
		virtual size_t CurrentIndex() = 0;
	};
}
