#pragma once
#include <cstddef>
#include <filesystem>
#include <memory>
#include "StaticConfig.hpp"
#include "PreWriter.hpp"

namespace FuncLib::Store
{
	using ::std::shared_ptr;
	using ::std::size_t;
	using ::std::filesystem::path;

	void WriteByte(path const& filename, pos_int start, char const* begin, size_t size);
	void DuplicateWriteByte(path const& filename, pos_int start, size_t count, char ch);

	class FileWriter
	{
	private:
		pos_int _pos;
		shared_ptr<path> _filename;
		bool _counterRunning = false;
		size_t _counter = 0;

	public:
		FileWriter(shared_ptr<path> filename, pos_int startPos);
		FileWriter(PreWriter preWriter, shared_ptr<path> filename, pos_int startPos);
		void StartCounter();
		void EndCounter();
		size_t CounterNum() const;
		void Write(char const* begin, size_t size);
		void WriteBlank(size_t count);
		~FileWriter();
	private:
		void Flush();
	};
}