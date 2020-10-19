#pragma once
#include <vector>
#include "StaticConfig.hpp"

namespace FuncLib::Store
{
	using ::std::size_t;
	using ::std::vector;

	class PreWriter
	{
	private:
		friend class FileWriter;
		vector<char> _buffer;
		bool _counterRunning = false;
		size_t _counter = 0;
	public:
		PreWriter();

		size_t Size() const;
		void Write(char const* begin, size_t size);
		void WriteBlank(size_t size);
		void StartCounter();
		void EndCounter();
		pos_int CounterNum() const;
		~PreWriter();
	private:
		void Flush();
	};
}
