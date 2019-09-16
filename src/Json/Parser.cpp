#include "Parser.hpp"

namespace Json {
	class O {
	private:
		const char Start = '[';
	public:
		static bool ValidBegin(char c)
		{
			// return true or false
		}
	};

	class Unknown {
	public:
		void
		Do(char c,/* should pass a iter with bound check function */ Status*& s, Json& json) // 还是建议每个函数里面不要动 i，Status 这个一般是自己这个对象，这样他就可以改变解析的方向了
		{

		}
	};

	Json
	_Parse()
	{

	}

	void TraverseChar()
	{
		auto str = "";
		auto status = Unknown();
		auto json = Json(); // should add a default constructor to Json

		for(auto i = 0; i < str.length(); ++i) {
			status.Do(c, status); // json?
		}
	}
}