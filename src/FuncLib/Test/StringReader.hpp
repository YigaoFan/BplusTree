#pragma once
#include <array>
#include <vector>
#include <string>
#include <sstream>

namespace FuncLib::Test
{
	using ::std::array;
	using ::std::byte;
	using ::std::move;
	using ::std::string;
	using ::std::stringstream;
	using ::std::vector;

	class StringReader
	{
	private:
		stringstream _stream;
		
	public:
		StringReader(string s)
			: _stream(move(s), stringstream::in | stringstream::binary)
		{ }

		vector<byte> Read(size_t size)
		{
			vector<byte> bytes(size);
			_stream.read(reinterpret_cast<char*>(&bytes), size);
			return bytes;
		}

		template <size_t N>
		array<byte, N> Read()
		{
			array<byte, N> bytes;
			_stream.read(reinterpret_cast<char *>(&bytes), N);
			return bytes;
		}

		void Skip(size_t size)
		{
			Read(size);
		}
	};
}
