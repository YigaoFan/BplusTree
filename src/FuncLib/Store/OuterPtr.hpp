#include <memory>
#include "File.hpp"

namespace FuncLib::Store
{
	using ::std::shared_ptr;

	// 这个类的作用和 DiskPtr 是不一样的，这个类是对读取出来的 object 的封装
	// 内外两种是不是可以自由的转换
	template <typename T>
	class OuterPtr
	{
	private:
		shared_ptr<T> _object;
		shared_ptr<File> _file;// 这个有作用的，对外使用的时候可以隐藏 File，并且拥有 File 的生命期用
		
	public:
		OuterPtr(OuterPtr const&) = delete;

		OuterPtr(OuterPtr&& that) noexcept
		{

		}

		void Delete()
		{
			// 没读 delete 会有问题，各个子部分都会出现这种情况
			// 可以构造个包含关系的树
		}

		// 因为 Store 本身带有保存 pos_label 不释放的能力，所以不能交给用户来控制
		~OuterPtr()
		{
			WriteObject();
		}

	private:
		void WriteObject()
		{
			_pos.WriteObject(_object);
		}

		/// caller should ensure wake all root element, j
		/// ust like a btree can wake all inner elements, but not other btree.
		// 放在 OuterPointer 那里实现
		// 存储位置的事就全由 OuterPointer 来负责了
		// void File::ReallocateContent()
		// {
		// 	// 还需要想很多，比如这一步很可能是需要其他步骤一起做才有效的
		// 	auto addr = 0; // reallocate from 0
		// 	for (auto& cacheKit : _cache)
		// 	{
		// 		shared_ptr<InsidePositionOwner> posOwner = get<3>(cacheKit);
		// 		posOwner->Addr(addr);
		// 		// addr += posOwner->RequiredSize();
		// 	}
		// }
	};
}
