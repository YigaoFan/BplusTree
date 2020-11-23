#pragma once
#include <vector>
#include "Store/StaticConfig.hpp"

namespace FuncLib
{
	using FuncLib::Store::pos_int;
	using ::std::vector;

	template <typename Callback>
	class BookingPos
	{
	private:
		friend class FuncBinaryLib;
		int _refCount = 0;
		pos_int _pos;
		vector<char> _bin;
		Callback _callbackOnDestroy;

	public:
		BookingPos(pos_int pos, Callback callbackOnDestroy)
			: _pos(pos), _callbackOnDestroy(move(callbackOnDestroy))
		{ }
		BookingPos(BookingPos const& that) = delete;

		~BookingPos()
		{
			_callbackOnDestroy(this);
		}
	};

	class FuncBinaryLib
	{
	private:
		pos_int _currentPos;

	public:
		FuncBinaryLib();
		template <typename T>
		BookingPos<T> Add(vector<char> bin)
		{
			return BookingPos<T>(_currentPos, [&](BookingPos<T>* pos)
			{
				// 这样搞就不允许多线程添加了
				if (not pos->_refCount == 0)
				{
					_currentPos += pos->_bin.size();
					// write to disk
				}
			});
		}

		template <typename T>
		void AddRefCount(BookingPos<T>& pos)
		{
			++pos._refCount;
		}

		// body 的大小存在 lib 文件里吧？
		vector<char> Read(pos_int pos, size_t size)
		{

		}
	};
	
	FuncBinaryLib::FuncBinaryLib()
	{
	}
}
