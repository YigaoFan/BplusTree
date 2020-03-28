#include <string>

namespace FuncLib
{
	using ::std::string;
	// Btree should have a type para to pass pointer type
	template <typename T>
	class PersistentPtr
	{

	};
	// Btree 中用到了几种数据？
	// 一种是实体，比如 Key，Value 最终在叶子节点存的都是实体，Node 之类应该是也是
	// 一种是存的指针，比如 unique_ptr，也就是说指针本身也要持久化
	template <typename T>
	class PersistentPtr<PersistentPtr<T>>
	{
	};

	template <>
	class PersistentPtr<string>
	{

	};

	template <>
	class PersistentPtr<int>
	{
	};
}