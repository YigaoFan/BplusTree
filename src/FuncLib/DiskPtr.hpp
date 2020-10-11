#pragma once
#include <memory>
#include <type_traits>
#include <vector>
#include <functional>
#include <cstddef>

namespace FuncLib
{
	using ::std::function;
	using ::std::is_same_v;
	using ::std::move;
	using ::std::shared_ptr;
	using ::std::vector;

	template <typename T>
	class DiskPtrBase
	{
	protected:
		template <typename Ty>
		friend class DiskPtrBase;
		
		mutable shared_ptr<vector<function<void(T*)>>> _setters;
		DiskPos<T> _pos;
		mutable shared_ptr<T> _tPtr;

	public:
		DiskPtrBase(DiskPos<T> pos, shared_ptr<T> object) : _pos(pos), _tPtr(object)
		{ }

		DiskPtrBase(DiskPtrBase&& that) noexcept
			: _tPtr(that._tPtr), _setters(move(that._setters)), _pos(move(that._pos))
		{
			that._tPtr = nullptr;
		}

		DiskPtrBase(DiskPtrBase const& that) : _tPtr(that._tPtr), _setters(that._setters), _pos(that._pos)
		{ }

		template <typename Derived>
		DiskPtrBase(DiskPtrBase<Derived>&& derivedOne) : _tPtr(derivedOne._tPtr), _pos(derivedOne._pos)
		{ 
			
		}

// setter 为空说明 object 处于最新状态，clear setter 保证这个
#define CLEAR_SETTER                    \
	if (!this->_setters->empty())       \
	{                                   \
		if (this->_tPtr == nullptr)     \
		{                               \
			this->ReadObjectFromDisk(); \
		}                               \
                                        \
		this->SetDone();                \
	}

		~DiskPtrBase()
		{
			CLEAR_SETTER;
		}

		DiskPtrBase& operator= (DiskPtrBase const& that)
		{
			CLEAR_SETTER;

			this->_tPtr = that._tPtr;
			this->_setters = that._setters;
			this->_pos = that._pos;

			return *this;
		}

		DiskPtrBase& operator= (DiskPtrBase&& that) noexcept
		{
			CLEAR_SETTER;

			this->_tPtr = move(that._tPtr);
			this->_setters = move(that._setters);
			this->_pos = move(that._pos);
			return *this;
		}
#undef CLEAR_SETTER

		void RegisterSetter(function<void(T*)> setter)
		{
			if (_tPtr == nullptr)
			{
				// 这里的 setter 这样存下来后，各地的 DiskPtr 从不同内存位置开始读的时候会不会有问题
				// 好像没有这个问题。obj 本身是缓存在 Cache 里的，且是共享的，那么有最新的状态，别人一定可以及时获知
				// 是考虑 Btree 的情况，还是考虑之后的所有情况？
				_setters->push_back(move(setter));
			}
			else
			{
				setter(_tPtr.get());
			}
		}

// prepare obj 保证对象读出来，且处于最新状态
#define PREPARE_OBJ                     \
	do                                  \
	{                                   \
		if (this->_tPtr == nullptr)     \
		{                               \
			this->ReadObjectFromDisk(); \
		}                               \
                                        \
		this->SetDone();                \
	} while (0)
		// 下面这些符号如果在实际的上层代码中能尽量不用就不用，因为涉及到读取
		T& operator* ()
		{
			PREPARE_OBJ;
			return *_tPtr;
		}

		T const& operator* () const
		{
			PREPARE_OBJ;
			return *_tPtr;
		}

		T* operator-> () const
		{
			PREPARE_OBJ;
			return _tPtr.get();
		}

		T* operator-> ()
		{
			PREPARE_OBJ;
			return _tPtr.get();
		}
#undef PREPARE_OBJ
	protected:
		DiskPtrBase(DiskPos<T> pos) : DiskPtrBase(move(pos), nullptr)
		{ }
		
	private:

		void ReadObjectFromDisk() const
		{
			_tPtr = _pos.ReadObject();
		}

		void SetDone() const
		{
			for (auto& f : *_setters)
			{
				f(_tPtr.get());
			}

			_setters->clear();
		}
	};

	template <typename T>
	class OwnerLessDiskPtr : public DiskPtrBase<T>
	{
	private:
		friend struct ByteConverter<OwnerLessDiskPtr, false>;
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

		template <typename T1>
		UniqueDiskPtr<T> MakeUnique(T1&& t, File* file)
		{
			// 硬存使用的出发点只有这里
			auto [lable, obj] = file->New(forward<T1>(t));
			return { obj, {file, lable } };
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
		// void reset(UniqueDiskPtr ptr)
		// {
		// }

		// T* get() const Unique 中提供这个功能，返回 OwnerLessDiskPtr
		// {
		// 	PREPARE_OBJ;
		// 	return _tPtr.get(); // TODO maybe modify use place
		// }
	};
}