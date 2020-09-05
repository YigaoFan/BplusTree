namespace FuncLib
{
	using pos_int = size_t;

	// 《设计模式》里说过一句叫解耦的方法就是加一层对象。这里是解 File 和 DiskPos<T> 的耦
	struct IFileAddr
	{
		virtual void Addr(pos_int newPos) = 0;
		virtual pos_int Addr() const = 0;
		virtual ~IFileAddr() = 0;
	};
}
