#include <vector>
#include <utility>
#include <string>
#include <string_view>

namespace Json::JsonConverter
{
	using ::std::vector;
	using ::std::pair;
	using ::std::string;
	using ::std::string_view;

	class StructObject
	{
	private:
		string _structName;
		// Public data member: type and name
		vector<pair<string, string>> _dataMembers;
	public:
		StructObject();
		StructObject(string structName);
		StructObject(string_view structName);
		void TypeName(string structName);
		string TypeName();
		void TypeName(string_view structName);
		void AppendDataMember(string type, string name);
		void AppendDataMember(string_view type, string_view name);
		string GetType(uint32_t i);
		string GetType(string const& name);
		string GetName(uint32_t i);
		pair<string, string> GetTypeNamePair(uint32_t i);
	};
}