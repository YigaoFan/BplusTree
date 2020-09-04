#include <memory>
#include <vector>
#include <cstddef>
#include <utility>
#include <cstdint>
#include "ByteConverter.hpp"
#include "../Basic/TypeTrait.hpp"

namespace FuncLib
{
	using ::Basic::IsSpecialization;
	using ::std::byte;
	using ::std::pair;
	using ::std::shared_ptr;
	using ::std::vector;

	using CacheId = ::std::uintptr_t;

	CacheId ComputeCacheId(shared_ptr<T> obj)
	{
		return reinterpret_cast<CacheId>(obj.get());
	}

	struct ICacheItemManager
	{
		virtual pair<pos_int, vector<byte>> RawDataInfo() const = 0;
		virtual bool Same(CacheId) const = 0;
		virtual ~ICacheItemManager() = 0;
	};

	class StoreWorker
	{
	private:
		shared_ptr<path> _filename;
		pos_int _pos;
	public:
		StoreWorker(shared_ptr<path> filename, pos_int pos)
			: _filename(filename), _pos(pos)
		{ }

		// how to gen data info when flush all
		template <typename T>
		pair<pos_int, vector<byte>> GenRawDataInfoOf(T const& t) const
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

		// bool Same(CacheId id) const
		// {
		// 	return ComputeCacheId(_cacheItem) == id;
		// }
	};
}
