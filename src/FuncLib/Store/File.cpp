#include "File.hpp"

namespace FuncLib::Store
{
	using ::std::get;
	using ::std::ofstream;

	set<File*> File::Files = {};

	shared_ptr<File> File::GetFile(path const &filename)
	{
		for (auto f : Files)
		{
			if (*(f->_filename) == filename)
			{
				return f->shared_from_this();
			}
		}

		auto f = make_shared<File>(filename, 0);
		Files.insert(f.get());
		f->_unloader = [f=f.get()]() 
		{
			Files.erase(f);
		};

		return f;
	}

	File::File(path const& filename, pos_int startPos)
		: _filename(make_shared<path>(filename)), _cache(_filename), _currentPos(startPos), _allocator(this)
	{ }

	void File::Flush()
	{
		// Flush 有点类似这整个析构一次的效果
		for (auto& cacheKit : _cache)
		{
			get<1>(cacheKit)();
		}
	}

	void File::ReallocateContent()
	{
		// 还需要想很多，比如这一步很可能是需要其他步骤一起做才有效的
		auto addr = 0; // reallocate from 0
		for (auto& cacheKit : _cache)
		{
			shared_ptr<InsidePositionOwner> posOwner = get<3>(cacheKit);
			posOwner->Addr(addr);
			// addr += posOwner->RequiredSize();
		}
	}

	shared_ptr<path> File::Path() const
	{
		return _filename;
	}

	File::~File()
	{
		_unloader();
	}
}