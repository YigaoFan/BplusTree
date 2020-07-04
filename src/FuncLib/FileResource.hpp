#pragma once
#include <memory>
#include "File.hpp"
#include "../Basic/Exception.hpp"

namespace FuncLib
{
	using ::std::shared_ptr;

	class FileResource
	{
	private:
		static thread_local shared_ptr<File> CurrentFile;

	public:
		static shared_ptr<File> GetCurrentThreadFile();

		static void SetCurrentThreadFile(shared_ptr<File> file);
	};
}