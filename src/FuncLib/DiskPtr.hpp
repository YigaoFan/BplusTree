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
		mutable shared_ptr<T> _tPtr = nullptr; // TODO Cache
		mutable DiskPos<T> _pos; // TODO mutable maybe has problem
		mutable vector<function<void(T*)>> _contentSetters;

	public:
		DiskPtrBase(shared_ptr<T> tPtr, DiskPos<T> pos) : _tPtr(tPtr), _pos(pos)
		{ }

		DiskPtrBase(DiskPos<T> pos) : _pos(pos)
		{ }

		DiskPtrBase(DiskPtrBase&& that)
			: _tPtr(that._tPtr), _pos(that._pos)
		{
			that._tPtr = nullptr;
		}

		DiskPtrBase& operator= (DiskPtrBase const &that)
		{
			this->_tPtr = that._tPtr;
			this->_pos = that._pos;
			if (!_contentSetters.empty())
			{
				// TODO do something
			}

			this->_contentSetters = that._contentSetters;
			return *this;
		}

		DiskPtrBase& operator= (DiskPtrBase&& that) noexcept
		{
			return *this;
		}

		// TODO test
		template <typename Derive, typename = enable_if_t<is_base_of_v<T, Derive>>>
		DiskPtrBase(DiskPtrBase<Derive>&& deriveOne)
			: _tPtr(deriveOne._tPtr), _pos(deriveOne._pos)
		{ }
		

		void RegisterSetter(function<void(T*)> setter)
		{
			if (_tPtr == nullptr)
			{
				_contentSetters.push_back(move(setter));
			}
			else
			{
				setter(_tPtr.get());
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
			// DiskPtr should support nullptr, compare to nullptr
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
	class WeakDiskPtr : public DiskPtrBase<T>
	{
	private:
		friend struct ByteConverter<WeakDiskPtr>;
		using Base = DiskPtrBase<T>;

	public:
		using Base::Base;
	};

	template <typename T>
	class DiskPtr : public DiskPtrBase<T>
	{
	private:
		friend struct ByteConverter<DiskPtr, false>;
		using Base = DiskPtrBase<T>;
	public:
		using Base::Base;

		static DiskPtr<T> MakeDiskPtr(shared_ptr<T> entityPtr, shared_ptr<File> file)
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

		// this ptr can destory data on disk
		DiskPtr(DiskPtr const&) = delete;

		DiskPtr& operator= (DiskPtr const& that)
		{
			Base::operator =(that);
			return *this;
		}

		DiskPtr& operator= (DiskPtr&& that) noexcept
		{
			Base::operator =(that);
			return *this;
		}

		DiskPtr(DiskPtr&& that)
			: Base(move(that))
		{

		}

		DiskPtr GetPtr() const
		{
			return { this->_tPtr, this->_pos };
		}

		void reset(DiskPtr ptr)
		{
			// if (ptr == nullptr)
			// release resource
			this->_tPtr = move(ptr->_tPtr);
			this->_pos = move(ptr->_pos);
			this->_contentSetters = move(ptr->_contentSetters);
		}
	};
}