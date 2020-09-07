#include <vector>
#include <memory>
#include <map>
#include <functional>
#include <filesystem>
#include <tuple>
#include "IInsidePositionOwner.hpp"

namespace FuncLib::Store
{
	using ::std::function;
	using ::std::make_unique;
	using ::std::map;
	using ::std::move;
	using ::std::shared_ptr;
	using ::std::tuple;
	using ::std::vector;
	using ::std::filesystem::path;

	using CacheId = ::std::uintptr_t;

	CacheId ComputeCacheId(shared_ptr<T> obj)
	{
		return reinterpret_cast<CacheId>(obj.get());
	}
	
	/// 一个 File 仅有一个 FileCache
	class FileCache
	{
	private:
		template <typename T>
		static map<path, map<size_t, shared_ptr<T>>> Cache;

		shared_ptr<path> _filename;
		function<void()> _unloader = []() {};
		// 将所有的 Cache Item 的功能都外显为此类的成员变量，比如
		// CacheId, store worker, cache remover
		vector<tuple<CacheId, function<void()>, function<void()>, shared_ptr<IInsidePositionOwner>>> _cacheKits;

	public:
		FileCache(shared_ptr<path> filename) : _filename(filename)
		{ }

		template <typename T>
		bool HasRead(size_t offset)
		{
			return Cache<T>.contains(_filename) && Cache<T>[_filename].contains(offset);
		}

		template <typename T>
		void Add(shared_ptr<IInsidePositionOwner> posOwner, shared_ptr<T> object)
		{
			if (!Cache<T>.contains(_filename))
			{
				_unloader = [&file = _filename, &fileCache = Cache<T>, previousUnloader = move(_unloader)]()
				{
					previousUnloader();
					fileCache.erase(file);
				};
			}

			using ::std::get_deleter;
			auto id = reinterpret_cast<CacheId>(object.get());
			auto storeWorker = [funPtr = get_deleter<void(T *)>(object), obj = object.get()]()
			{
				(*funPtr)(obj);
			};
			auto addr = posOwner->Addr();

			Cache<T>[*_filename].insert({ addr, object });

			auto remover = [file = _filename, =addr]()
			{
				Cache<T>[*file].erase(addr);
			};
			_cacheKits.push_back({ id, storeWorker, remover, posOwner });
		}

		template <typename T>
		void Remove(shared_ptr<T> object)
		{
			using ::std::get;

			for (auto it = _cacheKits.begin(); it < _cacheKits.end(); ++it)
			{
				auto& kit = *it;
				if (get<0>(kit) == ComputeCacheId(object))
				{
					// remove cache obj and cache will auto flush to disk
					get<2>(kit)();
					// remove cache kit
					_cacheKits.erase(it);
					return;
				}
			}
		}

		template <typename T>
		shared_ptr<T> Read(size_t offset)
		{
			return Cache<T>[_filename][offset];
		}

		auto begin()
		{
			return _cacheKits.begin();
		}

		auto end()
		{
			return _cacheKits.end();
		}

		auto begin() const
		{
			return _cacheKits.begin();
		}

		auto end() const
		{
			return _cacheKits.end();
		}

		/// Invoke unload inner. 
		/// Unload will remove cache item, which will invoke flush operation in object deleter.
		~FileCache()
		{
			_unloader();
		}
	};

	template <typename T>
	map<path, map<size_t, shared_ptr<T>>> FileCache::Cache = {};
}
