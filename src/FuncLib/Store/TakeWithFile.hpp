#pragma once
#include "../Persistence/Switch.hpp"
#include "File.hpp"

namespace FuncLib::Store
{
	using Persistence::Switch;

	// 这个类和 TakeWithDiskPos 的意义不一样，这个仅携带 File，而且是构造的时候就要传入设置
	// TakeWithDiskPos 是构造好再设置
	template <Switch SwitchState>
	class TakeWithFile;

	template <>
	class TakeWithFile<Switch::Enable>
	{
	private:
		File* _file;

	public:
		TakeWithFile(File* file) : _file(file) { }
		TakeWithFile(TakeWithFile &&) = default;
		TakeWithFile(TakeWithFile const&) = default;
		
		File* GetLessOwnershipFile() const
		{
			return _file;
		}
	};

	template <>
	class TakeWithFile<Switch::Disable>
	{ };
}
