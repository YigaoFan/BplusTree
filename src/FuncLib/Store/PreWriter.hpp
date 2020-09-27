#include <vector>
#include "StaticConfig.hpp"

namespace FuncLib::Store
{
	using ::std::size_t;

	class PreWriter// implement IWriter concept, 
	{
	private:
		/* data */
	public:
		PreWriter();
		size_t Size() const;
		void StartPos(pos_int pos);
		~PreWriter();
	};
	
	PreWriter::PreWriter(/* args */)
	{
	}
	
	PreWriter::~PreWriter()
	{
	}
	
}
