// #include "./Store/FileResource.hpp"

namespace FuncLib
{
	template <typename T>
	class DiskRef
	{
	private:
		friend struct ByteConverter<DiskRef, false>;
		OwnerLessDiskPtr<T> _ptr;
	public:
		// For temp conversion, will not write on Disk, is for user want to search some key, will use this
		// 类似下面这种类型转换应该显式的用 File 来构造
		DiskRef MakeRef(File* file)
		{
			
		}
		// DiskRef(T const& t) // TODO rewrite the implementation
		// 	: _ptr(UniqueDiskPtr<T>::MakeUnique(t, FileResource::GetCurrentThreadFile()))
		// { }

		DiskRef(OwnerLessDiskPtr<T> ptr) : _ptr(move(ptr))
		{ }

		DiskRef(DiskRef&& that) noexcept : _ptr(move(that._ptr))
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
