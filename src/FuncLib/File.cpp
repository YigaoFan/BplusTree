#include "File.hpp"

namespace FuncLib
{
	using ::std::ofstream;

	set<weak_ptr<File>, owner_less<weak_ptr<File>>> File::Files = {};

	File::File(path filename, pos_int startPos)
		: _filename(move(filename)), _cache(_filename), _currentPos(startPos)
	{ }

	void File::Flush()
	{
		ofstream fs(_filename, ofstream::binary);

		for (auto& cacheItem : _cache)
		{
			auto dataInfo = cacheItem->RawDataInfo();
			fs.seekp(dataInfo.first); // seekp and seekg diff?
			auto &bytes = dataInfo.second;

			// char not ensure to equal to byte size in standard
			fs.write((char *)(&bytes[0]), bytes.size());
		}

		fs.close();
	}

	vector<byte> File::Read(pos_int start, size_t size)
	{
		Flush();
		ifstream fs(_filename, ifstream::binary);
		fs.seekg(start);

		vector<byte> mem(size);

		if (fs.is_open())
		{
			fs.read(reinterpret_cast<char*>(&mem[0]), size);
		}
		
		return mem;
	}

	File::~File()
	{
		_unloader();
		Flush();
	}
}