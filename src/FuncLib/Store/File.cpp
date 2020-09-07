#include "File.hpp"

namespace FuncLib
{
	using ::std::get;
	using ::std::ofstream;

	set<weak_ptr<File>, owner_less<weak_ptr<File>>> File::Files = {};

	static shared_ptr<File> File::GetFile(path const &filename)
	{
		for (auto& weakFile : Files)
		{
			auto f = weakFile.lock();
			if (f->_filename == filename)
			{
				return f;
			}
		}

		auto f = make_shared<File>(filename);
		Files.insert(f);
		f->_unloader = [f = weak_ptr<File>(f)]() 
		{
			Files.erase(f);
		};
		return f;
	}

	File::File(path const& filename, pos_int startPos)
		: _filename(make_shared<path>(filename)), _cache(_filename), _currentPos(startPos)
	{ }

	void File::Flush()
	{
		for (auto& cacheKit : _cache)
		{
			get<1>(cacheKit)();
		}
	}

	vector<byte> File::Read(pos_int start, size_t size)
	{
		ifstream fs(_filename, ifstream::binary);
		fs.seekg(start);

		vector<byte> mem(size);

		if (fs.is_open())
		{
			fs.read(reinterpret_cast<char*>(&mem[0]), size);
		}
		
		return mem;
	}

	void File::ReallocateContent()
	{
		// 还需要想很多，比如这一步很可能是需要其他步骤一起做才有效的
		auto addr = 0; // reallocate from 0
		for (auto& cacheKit : _cache)
		{
			shared_ptr<IInsidePositionOwner>&& posOwner = get<3>(cacheKit);
			posOwner->Addr(addr);
			addr += posOwner->RequiredSize();
		}
	}

	File::~File()
	{
		_unloader();
	}
}