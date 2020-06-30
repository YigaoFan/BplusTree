#include "File.hpp"

namespace FuncLib
{
	using ::std::filesystem::path;
	using ::std::move;
	using ::std::ifstream;
	using ::std::ofstream;
	using ::std::vector;
	using ::std::byte;
	using ::std::move;

	File::File(path filename, pos_int startPos) : _filename(move(filename)), _currentPos(startPos)
	{ }

	void File::Flush()
	{
		if (!_bufferQueue.empty())
		{
			Filter();
			ofstream fs(_filename, ofstream::binary);

			for (auto& p : _bufferQueue)
			{
				auto offset = p.first;
				fs.seekp(offset);// seekp and seekg diff?
				auto& bytes = p.second;

				// char not ensure to equal to byte size
				fs.write((char*)(&bytes[0]), bytes.size());
			}

			fs.close();
		}
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
		Flush();
	}

	void File::Filter()
	{
		// sort
		// check first position duplicate
	}
}