class MemoryMemo
{
private:
	bool _hasValue = true;
	int _value;
public:
	static int Count;
	MemoryMemo();
	MemoryMemo(MemoryMemo&& that);
	MemoryMemo(MemoryMemo const& that);
	MemoryMemo& operator= (MemoryMemo const& that);
	MemoryMemo& operator= (MemoryMemo&& that);
	~MemoryMemo();
};