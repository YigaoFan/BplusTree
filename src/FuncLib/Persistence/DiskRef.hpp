#pragma once

namespace FuncLib::Persistence
{
	template <typename T>
	class UniqueDiskRef
	{
	private:
		friend struct ByteConverter<UniqueDiskRef, false>;
		friend struct OwnerLessDiskRef<T>;
		UniqueDiskPtr<T> _ptr;

	public:
		UniqueDiskRef(UniqueDiskPtr<T> ptr) : _ptr(move(ptr)) { }

		UniqueDiskRef(UniqueDiskRef&& that) noexcept : _ptr(move(that._ptr)) { }

		UniqueDiskRef& operator= (UniqueDiskRef const& that) = delete;

		UniqueDiskRef& operator= (UniqueDiskRef&& that) noexcept
		{
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
		OwnerLessDiskRef(UniqueDiskRef<T> const& uniqueRef) : _ptr(uniqueRef._ptr.Get())
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
