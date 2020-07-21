#include <memory>
#include "FileResource.hpp"

namespace FuncLib
{
	using ::std::make_shared;

	template <typename T>
	class DiskRef
	{
	private:
		friend struct ByteConverter<DiskRef, false>;
		UniqueDiskPtr<T> _ptr;// TODO replace with DiskPtrBase
	public:
		// For temp conversion, will not write on Disk, is for user want to search some key, will use this
		DiskRef(T const& t) // TODO rewrite the implementation
			: _ptr(UniqueDiskPtr<T>::MakeUnique(make_shared<T>(move(t)), FileResource::GetCurrentThreadFile()))
		{ }

		// want to Write on disk should use like this
		DiskRef(UniqueDiskPtr<T> ptr) : _ptr(move(ptr))
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
