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
		using ValueType = Item;
		virtual Item Current() = 0;
		virtual bool MoveNext() = 0;
		virtual size_t CurrentIndex() = 0;
	};
}
