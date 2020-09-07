#pragma once
#include <memory>
#include <type_traits>
#include <vector>
#include <functional>
#include "File.hpp"

namespace FuncLib
{
	using ::std::enable_if_t;
	using ::std::function;
	using ::std::is_base_of_v;
	using ::std::is_same_v;
	using ::std::move;
	using ::std::remove_const_t;
	using ::std::remove_cvref_t; // decay?
	using ::std::shared_ptr;
	using ::std::vector;

	template <typename T>
	class DiskPtrBase
	{
	protected:
		template <typename Ty>
		friend class DiskPtrBase;
		mutable shared_ptr<T> _tPtr;
		// mutable vector<function<void(T*)>> _contentSetters;
		shared_ptr<DiskPos<T>> _pos;

	public:
		DiskPtrBase(shared_ptr<T> tPtr, DiskPos<T> pos) : _tPtr(tPtr), _pos(shared_ptr(pos))
		{ }

		DiskPtrBase(DiskPos<T> pos) : _pos(shared_ptr(pos))
		{ }

		DiskPtrBase(DiskPtrBase&& that)
			: _tPtr(that._tPtr), _contentSetters(move(that._contentSetters)), _pos(move(that._pos))
		{
			that._tPtr = nullptr;
		}

		DiskPtrBase(DiskPtrBase const& that) : _tPtr(that._tPtr), _pos(that._pos)
		{
			that.DoSet();
		}

		DiskPtrBase& operator= (DiskPtrBase const& that)
		{
			// TODO handle the already exist _tPtr value
			that.DoSet();
			this->_tPtr = that._tPtr;// how to handle that._tPtr
			this->_pos = that._pos;

			return *this;
		}

		DiskPtrBase& operator= (DiskPtrBase&& that) noexcept
		{
			that.DoSet();
			this->_tPtr = that._tPtr;
			this->_pos = move(that._pos);
			return *this;
		}

		// TODO test
		template <typename Derive, typename = enable_if_t<is_base_of_v<T, Derive>>>
		DiskPtrBase(DiskPtrBase<Derive>&& deriveOne)
			: _tPtr(deriveOne._tPtr), _pos(deriveOne._pos)
		{ }
		
		void RegisterSetter(function<void(T*)> setter)
		{
			if (auto ptr = _cache.first; ptr == nullptr)
			{
				// 这里的 setter 这样存下来后，各地的 DiskPtr 从不同内存位置开始读的时候会不会有问题
				// 是考虑 Btree 的情况，还是考虑之后的所有情况？
				_cache.second.push_back(move(setter));
			}
			else
			{
				setter(ptr.get());
			}
		}

		// How to deal these operator
		T& operator* ()
		{
			ReadEntity();
			return *_tPtr;
		}

		T const& operator* () const
		{
			ReadEntity();
			return *_tPtr;
		}

		T* operator-> () const
		{
			ReadEntity();
			return _tPtr.get();
		}

		T* operator-> ()
		{
			ReadEntity();
			return _tPtr.get();
		}

		T* get() const
		{
			ReadEntity();
			return _tPtr.get();// TODO maybe modify use place
		}

		// bool operator!= ()
		// {
			// UniqueDiskPtr should support nullptr, compare to nullptr
			// TODO how to overload
		// }

		// bool operator< (DiskPtrBase const& that) const
		// {
		// 	// TODO
		// 	static_assert(true, "T type must can compare by <");
		// }

		shared_ptr<File> GetFile() const
		{
			return _pos.GetFile();
		}

		~DiskPtrBase()
		{
			if (_tPtr != nullptr)
			{
				_pos.WriteObject(_tPtr);// TODO judge if already write
			}
		}
	private:
		void ReadEntity() const
		{
			if (_tPtr == nullptr)
			{
				_tPtr = _pos.ReadObject();
			}

			DoSet();
		}

		void DoSet() const
		{
			if (!_contentSetters.empty())
			{
				for (auto& f : _contentSetters)
				{
					f(_tPtr.get());
				}

				_contentSetters.clear();
			}
		}
	};

	template <typename T>
	class SharedDiskPtr : public DiskPtrBase<T>
	{
	private:
		friend struct ByteConverter<SharedDiskPtr>;
		using Base = DiskPtrBase<T>;

	public:
		using Base::Base;
	};

	template <typename T>
	class UniqueDiskPtr : public DiskPtrBase<T>
	{
	private:
		friend struct ByteConverter<UniqueDiskPtr, false>;
		using Base = DiskPtrBase<T>;
	public:
		using Base::Base;

		static UniqueDiskPtr<T> MakeUnique(shared_ptr<T> entityPtr, shared_ptr<File> file)
		{
			size_t pos;
			if constexpr (is_same_v<typename ReturnType<decltype(ByteConverter<T>::ConvertToByte)>::Type, vector<byte>>)
			{
				auto d = ByteConverter<T>::ConvertToByte(*entityPtr);
				pos = file->Allocate<T>(d.size());
			}
			else
			{
				pos = file->Allocate<T>(ByteConverter<T>::Size);
			}

			return { entityPtr, { file, pos } }; // 硬存大小分配只有这里
		}

		UniqueDiskPtr(UniqueDiskPtr const&) = delete;

		UniqueDiskPtr& operator= (UniqueDiskPtr const& that)
		{
			Base::operator= (that);
			return *this;
		}

		UniqueDiskPtr& operator= (UniqueDiskPtr&& that) noexcept
		{
			Base::operator =(that);
			return *this;
		}

		UniqueDiskPtr(UniqueDiskPtr&& that)
			: Base(move(that))
		{

		}

		UniqueDiskPtr GetPtr() const
		{
			return { this->_tPtr, this->_pos };
		}

		void reset(UniqueDiskPtr ptr)
		{
			// if (ptr == nullptr)
			// release resource
			this->_tPtr = move(ptr->_tPtr);
			this->_pos = move(ptr->_pos);
			this->_contentSetters = move(ptr->_contentSetters);
		}
	};
}