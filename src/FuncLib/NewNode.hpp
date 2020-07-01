namespace FuncLib
{
	auto NewNode(auto oldNodePtr)
	{
		using ::FuncLib::DiskPtr;
		using ::std::decay_t;
		using Type = decay_t<decltype(oldNodePtr)>;
		if constexpr (IsSpecialization<Type, DiskPtr>::value)
		{
			auto file = OLD_NODE_PTR.GetFile();
			return DiskPtr<remove_pointer_t<decltype(oldNodePtr)>>::MakeDiskPtr(_elements.LessThanPtr);
		}
		else
		{
			return make_unique<remove_pointer_t<decltype(this)>>(_elements.LessThanPtr);
		}
	}
}  