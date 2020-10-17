namespace FuncLib
{
	template <typename T>
	class UniqueDiskRef
	{
	private:
		friend struct ByteConverter<UniqueDiskRef, false>;
		friend struct OwnerLessDiskRef<T>;
		UniqueDiskPtr<T> _ptr;

	public:
		UniqueDiskRef(T const& t) : _ptr(DiskPos<T>(), nullptr)
		{ }// for compile only

		UniqueDiskRef(UniqueDiskPtr<T> ptr) : _ptr(move(ptr)) { }

		UniqueDiskRef(UniqueDiskRef&& that) noexcept : _ptr(move(that._ptr)) { }

		UniqueDiskRef(UniqueDiskRef const& that) : _ptr(that._ptr.Clone())
		{ }

		UniqueDiskRef& operator= (UniqueDiskRef const& that) = delete;

		// 这个函数到底可不可以删
		UniqueDiskRef& operator= (UniqueDiskRef&& that) noexcept
		{
			// 这里支持右值赋值，但是原来 this 保存的对象还没有处理 TODO
			this->_ptr = move(that._ptr);
			return *this;
		}

		operator T& () { return *_ptr; }
		operator T const& () const { return *_ptr; }
	};

	template <typename T>
	class OwnerLessDiskRef
	{
	private:
		friend struct ByteConverter<OwnerLessDiskRef, false>;
		OwnerLessDiskPtr<T> _ptr;
	public:
		OwnerLessDiskRef(T const& t)
		{ }// for compile only

		OwnerLessDiskRef(UniqueDiskRef<T> const &uniqueRef) : _ptr(uniqueRef._ptr.Get())
		{ }

		OwnerLessDiskRef(OwnerLessDiskPtr<T> ptr) : _ptr(move(ptr)) { }

		OwnerLessDiskRef(OwnerLessDiskRef&& that) noexcept : _ptr(move(that._ptr)) { }

		OwnerLessDiskRef(OwnerLessDiskRef const& that) : _ptr(that._ptr) { }

		OwnerLessDiskRef& operator= (OwnerLessDiskRef const& that)
		{
			this->_ptr = that._ptr;
			return *this;
		}

		OwnerLessDiskRef& operator= (OwnerLessDiskRef&& that) noexcept
		{
			this->_ptr = move(that._ptr);
			return *this;
		}

		operator T& () { return *_ptr; }
		operator T const& () const { return *_ptr; }
	};
}
