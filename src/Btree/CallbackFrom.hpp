#include <functional>
#include "../FuncLib/Persistence/FriendFuncLibDeclare.hpp"
#include "TypeConfig.hpp"

namespace Collections
{
	using FuncLib::Persistence::OwnerLessDiskPtr;
	using ::std::forward;
	using ::std::function;
	using ::std::move;
	
	// 为之后可能出现的情况准备的类，由子类对象想调用父类对象设置的回调，但父类对象尚未从硬盘里激活时使用这个
	// 代替所有的回调类型，并做一些小改动，还有在父类对象由上级激活读取并去 cache 里面找时，nullptr 化此对象 DiskPtr 里
	// 保存的父类对象 shared_ptr 引用——可以在 cache 里重复读 cache 的地方开洞，让各类型特化出各自的处理代码
	// 防止引用关系成环
	template <StorePlace Place, typename T, typename Ret, typename... Args>
	class CallbackFrom;

	template <typename T, typename Ret, typename... Args>
	class CallbackFrom<StorePlace::Memory, T, Ret, Args...>
	{
	private:
		function<Ret(Args...)> _callback;

	public:
		CallbackFrom(function<Ret(Args...)> callback) : _callback(move(callback))
		{
		}

		template <typename Derived>
		CallbackFrom(CallbackFrom<Derived>&& that) : _callback(move(that._callback))
		{
		}

		CallbackFrom& operator= (CallbackFrom&& that) noexcept
		{
			this->_callback = move(that._callback);
			return *this;
		}

		Ret operator() (Args... args)
		{
			return _callback(forward<Args>(args)...);
		}
	};

	template <typename T, typename Ret, typename... Args>
	class CallbackFrom<StorePlace::Disk, T, Ret, Args...> : private CallbackFrom<StorePlace::Memory, T, Ret, Args...>
	{
	private:
		using Base = CallbackFrom<StorePlace::Memory, T, Ret, Args...>;
		OwnerLessDiskPtr<T> _ptr;

	public:
		CallbackFrom(OwnerLessDiskPtr<T> ptr, function<Ret(Args...)> callback)
			: _ptr(move(ptr)), Base(move(callback))
		{ }

		template <typename Derived>
		CallbackFrom(CallbackFrom<Derived>&& that)
			: Base(move(that._callback)), _ptr(move(that._ptr))
		{ }

		CallbackFrom& operator= (CallbackFrom&& that) noexcept
		{
			Base::operator= (move(that));
			this->_ptr = move(that._ptr);
			return *this;
		}

		using Base::operator();
	};
}
