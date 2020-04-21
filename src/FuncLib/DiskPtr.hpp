#pragma once
#include <memory>
#include <type_traits>
#include <vector>
#include <functional>
#include "DiskPos.hpp"
#include "File.hpp"
#include "../Basic/TypeTrait.hpp"

namespace FuncLib
{
	using ::std::vector;
	using ::std::function;
	using ::std::shared_ptr;
	using ::std::move;
	using ::std::enable_if_t;
	using ::std::is_base_of_v;
	using ::std::remove_cvref_t;// decay?
	using ::Basic::IsSpecialization;

	template <typename T>
	class DiskPtrBase
	{
	protected:
		shared_ptr<T> _tPtr = nullptr; // TODO Cache
		DiskPos<T> _pos;
		vector<function<void(T*)>> _contentSetters;

	public:
		DiskPtrBase(shared_ptr<T> pos) : _pos(pos)
		{ }

		DiskPtrBase(DiskPos<T> pos) : _pos(pos)
		{
		}

		// TODO test
		template <typename Derive, typename = enable_if_t<is_base_of_v<T, Derive>>>
		DiskPtrBase(DiskPtrBase<Derive>&& deriveOne)
			: _tPtr(deriveOne._tPtr), _pos(deriveOne._pos)
		{ }
		
		void RegisterSetter(function<void(T*)> callback)
		{
			_contentSetters.push_back(move(callback));
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
		friend struct ByteConverter<DiskPtr>;
		using Base = DiskPtrBase<T>;
	public:
		using Base::Base;

		static DiskPtr<T> MakeDiskPtr(shared_ptr<T> entityPtr, shared_ptr<File> file)
		{
			return file->Allocate<T>(ByteConverter<T>::ConvertToByte(*entityPtr));// 硬存大小分配只有这里
		}

		// this ptr can destory data on disk
		DiskPtr(DiskPtr& const) = delete;
		WeakDiskPtr<T> GetWeakPtr()
		{

		}
	};

	template <typename Ptr>
	struct GetContentType;

	template <typename T>
	struct GetContentType<unique_ptr<T>>
	{
		using Type = T;
	};

	template <typename T>
	struct GetContentType<DiskPtr<T>>
	{
		using Type = T;
	};

	template <typename T>
	struct GetContentType<T*>
	{
		using Type = T;
	};

	template <typename Ptr>
	void SetProperty(Ptr& t, function<void(typename GetContentType<Ptr>::Type*)> setter)
	{
		using Type = remove_cvref_t<Ptr>;
		if constexpr (IsSpecialization<Type, unique_ptr>::value)
		{
			setter(t.get());
		}
		else
		{
			static_assert(IsSpecialization<Type, DiskPtr>::value);
			t.RegisterSetter(setter);
		}
	}

	//	// 这里想到了一个问题，如何防止一个地方被重复读取
	//	// 即硬盘里的都是原始的，需要还原的，但每个都还原，就会重复了
	//	// 用缓存？

	// Update content to disk
	// Or register update event in allocator, 然后集中更新
	// Use DiskPos to update
}