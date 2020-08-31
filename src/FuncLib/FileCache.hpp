#include <vector>
#include <memory>
#include <utility>
#include <map>
#include <functional>
#include <filesystem>
#include "../Basic/Exception.hpp"

namespace FuncLib
{
	using ::Basic::InvalidAccessException;
	using ::std::function;
	using ::std::make_pair;
	using ::std::map;
	using ::std::move;
	using ::std::pair;
	using ::std::shared_ptr;
	using ::std::vector;
	using ::std::weak_ptr;
	using ::std::filesystem::path;

	class FileCache
	{
	private:
		template <typename T>
		static map<path, map<size_t, weak_ptr<T>>> Cache;

		path const& _filename;
		function<void()> _unloader;
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

			Cache<T>.insert(make_pair<path>(_filename, { offset, object }));
		}

		template <typename T>
		shared_ptr<T> Read(size_t offset)
		{
			// release version code should without check
			auto& weakPtr = Cache<T>[_filename][offset];
			if (weakPtr.expired())
			{
				throw InvalidAccessException
					("read expired object in cache. please check code logic");
			}
			else
			{
				return weakPtr.lock();
			}
		}

		~FileCache()
		{
			_unloader();
		}
	};

	template <typename T>
	map<path, map<size_t, weak_ptr<T>>> FileCache::Cache = {};
}
