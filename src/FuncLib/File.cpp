#include "File.hpp"

namespace FuncLib
{
	using ::std::filesystem::path;
	using ::std::move;
	using ::std::ifstream;
	using ::std::ofstream;
	using ::std::fstream;
	using ::std::vector;
	using ::std::pair;
	using ::std::byte;
	using ::std::function;
	using ::std::string;
	using ::std::move;
	using ::std::size_t;
	using ::std::ios_base;

	File::File(path filename, size_t startPos) : _filename(move(filename)), _currentPos(startPos)
	{ }
		
	shared_ptr<File> File::GetPtr()
	{
		return shared_from_this();
	}

	void File::Flush()
	{
		if (!_bufferQueue.empty())
		{
			Filter();
			// TODO test
			//ofstream fs(_filename, ios_base::out | ios_base::in | ios_base::binary);
			// difference?
			fstream fs(_filename, ios_base::out | ios_base::in | ios_base::binary);
			for (auto& p : _bufferQueue)
			{
				auto offset = p.first;
				//fs.seekp ?
				fs.seekg(offset);
				auto& bytes = p.second;

				// char not ensure to equal to byte
				fs.write((char*)(&bytes[0]), bytes.size());
			}

			fs.close();
		}
	}

	vector<byte> File::Read(uint32_t start, uint32_t size)
	{
		ifstream fs(_filename, ifstream::binary | ifstream::in);
		fs.seekg(start);

		char c;
		vector<byte> mem;
		mem.reserve(size);

		for (auto i = 0; i < size && fs.get(c); ++i)
		{
			mem.push_back(static_cast<byte>(c));
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