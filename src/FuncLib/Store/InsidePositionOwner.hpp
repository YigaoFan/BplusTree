#pragma once
#include <cstddef>
#include <memory>
#include "StaticConfig.hpp"

namespace FuncLib::Store
{
	using ::std::shared_ptr;
	using ::std::size_t;

	class InsidePositionOwner
	{
	private:
		pos_int _pos;
		size_t  _size;
	protected:
		InsidePositionOwner();
		InsidePositionOwner(pos_int pos);
		InsidePositionOwner(pos_int pos, size_t size);
	public:
		void Addr(pos_int newPos);
		pos_int Addr() const;
		size_t Size() const;
		void Size(size_t size);
		virtual ~InsidePositionOwner() = default;
	};

	class File;
	template <typename T, typename... Ts>
	shared_ptr<InsidePositionOwner> MakePositionOwner(Ts... ts);
}
