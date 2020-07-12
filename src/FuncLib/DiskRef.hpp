#include <memory>
// #include "ByteConverter.hpp"
#include "FileResource.hpp"

namespace FuncLib
{
	using ::std::make_shared;

	template <typename T>
	class DiskRef
	{
	private:
		friend struct ByteConverter<DiskRef, false>;
		friend struct ByteConverter<DiskRef, true>;
		DiskPtr<T> _ptr;
	public:
		DiskRef(T t)
			: _ptr(DiskPtr<T>::MakeDiskPtr(make_shared<T>(move(t)), FileResource::GetCurrentThreadFile()))
		{ }

		DiskRef(DiskPtr<T> ptr) : _ptr(move(ptr))
		{ }

		DiskRef(DiskRef&& that) : _ptr(move(that._ptr))
		{ }

		DiskRef(DiskRef const& that) : _ptr(that._ptr.GetPtr())
		{ }

		DiskRef& operator= (DiskRef const& that)
		{
			this->_ptr = that._ptr;
			return *this;
		}

		DiskRef& operator= (DiskRef&& that) noexcept
		{
			this->_ptr = move(that._ptr);
			return *this;
		}

		operator T& ()
		{
			return *_ptr;
		}

		operator T const& () const
		{
			return *_ptr;
		}
	};
}
