namespace FuncLib
{
	using pos_int = size_t;

	class FileAddr
	{
	private:
		pos_int _addr;
	public:
		FileAddr(pos_int addr) : _addr(addr)
		{ }

		void Addr(pos_int newAddr)
		{ 
			_addr = newAddr;
		}

		pos_int Addr() const
		{
			return _addr;
		}
	};
}
