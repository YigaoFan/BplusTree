#include <memory>
#include "File.hpp"
#include "../ByteConverter.hpp"

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
		void WriteObject()
		{
			_pos.WriteObject(_object);
		}
	};
}
