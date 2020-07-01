#pragma once
#include <memory>
#include <type_traits>
#include <vector>
#include <functional>
#include "File.hpp"

namespace FuncLib
{
	using ::std::move;
	using ::std::is_same_v;
	using ::std::vector;
	using ::std::function;
	using ::std::shared_ptr;
	using ::std::enable_if_t;
	using ::std::is_base_of_v;
	using ::std::remove_cvref_t;// decay?

	template <typename T>
	class DiskPtrBase
	{
	protected:
		shared_ptr<T> _tPtr = nullptr; // TODO Cache
		DiskPos<T> _pos;
		vector<function<void(T*)>> _contentSetters;

	public:
		DiskPtrBase(shared_ptr<T> tPtr, DiskPos<T> pos) : _tPtr(tPtr), _pos(pos)
		{ }

		DiskPtrBase(DiskPos<T> pos) : _pos(pos)
		{ }

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

		T& operator* ()
		{
			ReadEntity();
			return *_tPtr;
		}

		T* operator-> ()
		{
			ReadEntity();
			return _tPtr;
		}

		// bool operator!= ()
		// {
			// DiskPtr should support nullptr, compare to nullptr
			// TODO how to overload
		// }

		operator T& () const
		{
			ReadEntity();
			return *_tPtr;
		}

		~DiskPtrBase()
		{
			if (_tPtr != nullptr)
			{
				_pos.WriteObject(_tPtr);
			}
		}
	private:
		void ReadEntity()
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
				pos = file->Write<T>(ByteConverter<T>::ConvertToByte(*entityPtr));
			}
			else
			{
				pos = file->Allocate(ByteConverter<T>::Size);
			}

			return { entityPtr, { file, pos } }; // 硬存大小分配只有这里
		}

		// this ptr can destory data on disk
		DiskPtr(DiskPtr const&) = delete;

		WeakDiskPtr<T> GetWeakPtr()
		{
			throw 1;
		}

		void reset(DiskPtr ptr)
		{
			// release resource
			this->_tPtr = move(ptr->_tPtr);
			this->_pos = move(ptr->_pos);
			this->_contentSetters = move(ptr->_contentSetters);
		}
	};
}