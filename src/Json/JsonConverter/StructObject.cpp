#include "StructObject.hpp"
#include "../../Basic/Exception.hpp"

namespace Json::JsonConverter
{
	using ::std::move;

	string ConsStringFromView(string_view view)
	{
		return { view.data(), view.size() };
	}

	StructObject::StructObject()
	{ }

	StructObject::StructObject(string structName)
		: _typeName(move(structName))
	{ }

	StructObject::StructObject(string_view structName)
		: _typeName(ConsStringFromView(structName))
	{ }

	void StructObject::TypeName(string structName)
	{
		_typeName = move(structName);
	}

	void StructObject::TypeName(string_view structName)
	{
		_typeName = ConsStringFromView(structName);
	}

	string StructObject::TypeName() const
	{
		return _typeName;
	}

	void StructObject::AppendDataMember(string type, string name)
	{
		_dataMembers.push_back({ move(type), move(name) });
	}

	void StructObject::AppendDataMember(string_view type, string_view name)
	{
		AppendDataMember(ConsStringFromView(type), ConsStringFromView(name));
	}

	string StructObject::GetType(uint32_t i) const
	{
		return _dataMembers[i].first;
	}

	string StructObject::GetType(string const& name) const
	{
		using Basic::KeyNotFoundException;

		for (auto& p : _dataMembers)
		{
			if (p.second == name)
			{
				return p.first;
			}
		}

		throw KeyNotFoundException(name);
	}

	string StructObject::GetName(uint32_t i) const
	{
		return _dataMembers[i].second;
	}

	pair<string, string> StructObject::GetTypeNamePair(uint32_t i) const
	{
		return _dataMembers[i];
	}

	vector<pair<string, string>> StructObject::DataMembers() const
	{
		return _dataMembers;
	}
}