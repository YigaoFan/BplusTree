#include <memory>
#include <vector>
#include <cstddef>
#include <utility>
#include "ByteConverter.hpp"
#include "../Basic/TypeTrait.hpp"

namespace FuncLib
{
	using ::Basic::IsSpecialization;
	using ::std::byte;
	using ::std::pair;
	using ::std::shared_ptr;
	using ::std::vector;

	struct ICacheItemManager
	{
		virtual pair<pos_int, vector<byte>> RawDataInfo() const = 0;
		virtual ~ICacheItemManager() = 0;
	};

	template <typename T>
	class CacheItemManager : public ICacheItemManager
	{
	private:
		pos_int _pos;
		shared_ptr<T> _cacheItem;
	public:
		CacheItemManager(pos_int pos, shared_ptr<T> cacheItem)
			: _pos(pos), _cacheItem(cacheItem)
		{ }

		pair<pos_int, vector<byte>> RawDataInfo() const override
		{
			auto bytes = ByteConverter<T>::ConvertToByte(*_cacheItem);
			if constexpr (IsSpecialization<decltype(bytes), vector>::value)
			{
				return { _pos, bytes };
			}
			else
			{
				return { _pos, { bytes.begin(), bytes.end() }};
			}
		}
	};
}
