#include <vector>
#include <memory>
#include "FileAddr.hpp"

namespace FuncLib
{
	using ::std::vector;
	using ::std::shared_ptr;

	class AddrManager
	{
	private:
		vector<shared_ptr<FileAddr>> _addrs;
	public:
		AddrManager();

	};
	
	AddrManager::AddrManager(/* args */)
	{
	}
	
	AddrManager::~AddrManager()
	{
	}	
}
