#pragma once
#include <memory>
#include <type_traits>
#include <vector>
#include <functional>

namespace FuncLib
{
	using ::std::function;
	using ::std::is_same_v;
	using ::std::move;
	using ::std::remove_const_t;
	using ::std::remove_cvref_t; // decay?
	using ::std::shared_ptr;
	using ::std::vector;

	// 如何区分删除和析构
	template <typename T>
	class DiskPtrBase
	{
	protected:
		template <typename Ty>
		friend class DiskPtrBase;
		
		mutable shared_ptr<T> _tPtr = nullptr;
		mutable vector<function<void(T*)>> _setters;
		shared_ptr<DiskPos<T>> _pos;

	public:
		DiskPtrBase(DiskPos<T> pos) : _pos(shared_ptr(pos))
		{ }

		DiskPtrBase(DiskPtrBase&& that)
			: _tPtr(that._tPtr), _setters(move(that._setters)), _pos(move(that._pos))
		{
			that._tPtr = nullptr;
			that._pos = nullptr;
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
			this->_tPtr = move(that._tPtr);
			this->_pos = move(that._pos);
			return *this;
		}

		// TODO test
		template <typename Derive>
		DiskPtrBase(DiskPtrBase<Derive>&& deriveOne)
			: _tPtr(deriveOne._tPtr), _pos(deriveOne._pos)
		{ }
		
		void RegisterSetter(function<void(T*)> setter)
		{
			if (_tPtr == nullptr)
			{
				// 这里的 setter 这样存下来后，各地的 DiskPtr 从不同内存位置开始读的时候会不会有问题
				// 是考虑 Btree 的情况，还是考虑之后的所有情况？
				_setters.push_back(move(setter));
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

		shared_ptr<File> GetFile() const
		{
			return _pos->GetFile();
		}
	private:
		void ReadEntity() const
		{
			if (_tPtr == nullptr)
			{
				_tPtr = _pos->ReadObject();
			}

			DoSet();
		}

		void DoSet() const
		{
			if (!_setters.empty())
			{
				for (auto& f : _setters)
				{
					f(_tPtr.get());
				}

				_setters.clear();
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
			// size_t pos;
			// if constexpr (is_same_v<typename ReturnType<decltype(ByteConverter<T>::WriteDown)>::Type, vector<byte>>)
			// {
			// 	auto d = ByteConverter<T>::WriteDown(*entityPtr);
			// 	pos = file->Allocate<T>(d.size());
			// }
			// else
			// {
			// 	pos = file->Allocate<T>(ByteConverter<T>::Size);
			// }

			// return { entityPtr, { file, pos } }; // 硬存大小分配只有这里
		}

		UniqueDiskPtr(UniqueDiskPtr const&) = delete;

		UniqueDiskPtr& operator= (UniqueDiskPtr const& that) = delete;

		UniqueDiskPtr& operator= (UniqueDiskPtr&& that) noexcept
		{
			Base::operator =(that);
			return *this;
		}

		UniqueDiskPtr(UniqueDiskPtr&& that) : Base(move(that)) { }

		UniqueDiskPtr GetPtr() const
		{
			return { this->_tPtr, this->_pos };
		}

		// 哪里用到了这个？
		void reset(UniqueDiskPtr ptr)
		{
			// if (ptr == nullptr)
			// release resource
			this->_tPtr = move(ptr->_tPtr);
			this->_pos = move(ptr->_pos);
			this->_setters = move(ptr->_setters);
		}
	};
}