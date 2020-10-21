#include <memory>
#include "File.hpp"

namespace FuncLib::Store
{
	using ::std::shared_ptr;

	// 内外两种是不是可以自由的转换
	template <typename T>
	class OuterDiskPtr : public DiskPtrBase
	{
	private:
		shared_ptr<File> _file;// 这个应该没有什么作用，只是掌握 File 的生命期用的
		
	public:
		OuterDiskPtr();

		void Delete()
		{

		}

		// 因为 Store 本身带有保存 pos_lable 不释放的能力，所以不能交给用户来控制
		~OuterDiskPtr()
		{
			WriteObject();
		}

	private:
		void WriteObject()
		{
			_pos.WriteObject(_object);
		}

		// 这个功能可能不要了，可能放在 OuterPointer 那里实现了
		// void File::Flush()
		// {
		// 	// Flush 有点类似这整个析构一次的效果
		// 	for (auto& cacheKit : _cache)
		// 	{
		// 		get<1>(cacheKit)();
		// 	}
		// }

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
