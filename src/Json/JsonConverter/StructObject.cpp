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
		: _structName(move(structName))
	{ }

	StructObject::StructObject(string_view structName)
		: _structName(ConsStringFromView(structName))
	{ }

	void StructObject::StructName(string structName)
	{
		_structName = move(structName);
	}

	void StructObject::StructName(string_view structName)
	{
		_structName = ConsStringFromView(structName);
	}

	string StructObject::StructName()
	{
		return _structName;
	}

	void StructObject::AppendDataMember(string type, string name)
	{
		_dataMembers.push_back({ move(type), move(name) });
	}

	void StructObject::AppendDataMember(string_view type, string_view name)
	{
		AppendDataMember(ConsStringFromView(type), ConsStringFromView(name));
	}

	string StructObject::GetType(uint32_t i)
	{
		return _dataMembers[i].first;
	}

	string StructObject::GetType(string const& name)
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

	string StructObject::GetName(uint32_t i)
	{
		return _dataMembers[i].second;
	}

	pair<string, string> StructObject::GetTypeNamePair(uint32_t i)
	{
		return _dataMembers[i];
	}
}