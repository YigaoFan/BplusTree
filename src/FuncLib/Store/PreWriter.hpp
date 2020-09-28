#include <vector>
#include "StaticConfig.hpp"

namespace FuncLib::Store
{
	using ::std::size_t;
	using ::std::vector;

	class PreWriter
	{
	private:
		pos_int _startPos;
		vector<char> _buffer;
		bool _counterRunning = false;
		size_t _counter = 0;
	public:
		PreWriter();
		size_t Size() const;
		void StartPos(pos_int pos);
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
