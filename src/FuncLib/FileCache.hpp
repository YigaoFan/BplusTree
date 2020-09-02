#include <vector>
#include <memory>
#include <utility>
#include <map>
#include <functional>
#include <filesystem>
#include "ICacheItemManager.hpp"

namespace FuncLib
{
	using ::std::function;
	using ::std::make_pair;
	using ::std::map;
	using ::std::move;
	using ::std::pair;
	using ::std::shared_ptr;
	using ::std::unique_ptr;
	using ::std::make_unique;
	using ::std::vector;
	using ::std::filesystem::path;

	class FileCache
	{
	private:
		template <typename T>
		static map<path, map<size_t, shared_ptr<T>>> Cache;

		path const& _filename;
		function<void()> _unloader;
		vector<unique_ptr<ICacheItemManager>> _managers;
		// remember to delete content when call File::Delete
		// shared_ptr == nullptr is delete? for convenient
	public:
		FileCache(path const& filename) : _filename(filename)
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

			_managers.push_back(make_unique<ICacheItemManager>(object));
			Cache<T>.insert(make_pair<path>(_filename, { offset, object }));
		}

		template <typename T>
		shared_ptr<T> Read(size_t offset)
		{
			return Cache<T>[_filename][offset];
		}

		auto begin()
		{
			return _managers.begin();
		}

		auto end()
		{
			return _managers.end();
		}

		auto begin() const
		{
			return _managers.begin();
		}

		auto end() const
		{
			return _managers.end();
		}

		~FileCache()
		{
			_unloader();
		}
	};

	template <typename T>
	map<path, map<size_t, shared_ptr<T>>> FileCache::Cache = {};
}
