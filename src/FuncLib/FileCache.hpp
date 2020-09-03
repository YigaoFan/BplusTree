#include <vector>
#include <memory>
#include <utility>
#include <map>
#include <functional>
#include <filesystem>
#include <tuple>
#include "ICacheItemManager.hpp"

namespace FuncLib
{
	using ::std::function;
	using ::std::make_pair;
	using ::std::map;
	using ::std::move;
	using ::std::pair;
	using ::std::shared_ptr;
	using ::std::make_unique;
	using ::std::vector;
	using ::std::filesystem::path;
	using ::std::tuple;

	class FileCache
	{
	private:
		// use a 1to1 container?
		template <typename T>
		static map<path, map<size_t, shared_ptr<T>>> Cache;

		shared_ptr<path> _filename;
		function<void()> _unloader = []() {};
		// 将所有的 Cache Item 的功能都外显为此类的成员变量，比如
		// CacheId, store worker, cache remover
		vector<tuple<CacheId, shared_ptr<function<void()>>, function<void()>>> _cacheKits;
		// store worker inner use raw pointer directly, not use shared_ptr which will come to circle ref
		// 是 shared_ptr 是让 object 里面也共用这个

	public:
		FileCache(shared_ptr<path> filename) : _filename(filename)
		{ }

		template <typename T>
		bool HasRead(size_t offset)
		{
			return Cache<T>.contains(_filename) && Cache<T>[_filename].contains(offset);
		}

		template <typename T>
		void Add(size_t offset, shared_ptr<T> object)
		{
			if (!Cache<T>.contains(_filename))
			{
				_unloader = [&file = _filename, &fileCache = Cache<T>, previousUnloader = move(_unloader)]()
				{
					previousUnloader();
					fileCache.erase(file);
				};
			}

			_storeWorkers.push_back(make_unique<ICacheItemManager>(object));
			Cache<T>.insert(make_pair<path>(_filename, { offset, object }));
		}

		template <typename T>
		void Remove(shared_ptr<T> object)
		{
			// remove Cache item

			for (auto it = _storeWorkers.begin(); it < _storeWorkers.end(); ++it)
			{
				if ((*it)->Same(ComputeCacheId(object)))
				{
					_storeWorkers.erase(it);
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
			return _storeWorkers.begin();
		}

		auto end()
		{
			return _storeWorkers.end();
		}

		auto begin() const
		{
			return _storeWorkers.begin();
		}

		auto end() const
		{
			return _storeWorkers.end();
		}

		~FileCache()
		{
			_unloader();
		}
	};

	template <typename T>
	map<path, map<size_t, shared_ptr<T>>> FileCache::Cache = {};
}
