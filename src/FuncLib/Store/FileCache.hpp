#include <memory>
#include <map>
#include <functional>
#include "StaticConfig.hpp"

namespace FuncLib::Store
{
	using ::std::function;
	using ::std::map;
	using ::std::move;
	using ::std::shared_ptr;


	/// 一个 File 仅有一个 FileCache
	class FileCache
	{
	public:
		using id_int = unsigned int;
	private:
		template <typename T>
		/// file id, pos lable, object
		static map<id_int, map<pos_lable, shared_ptr<T>>> Cache;

		id_int _fileId;
		function<void()> _unloader = []() {};
	public:
		FileCache(id_int fileId);
		~FileCache();

		template <typename T>
		bool Cached(pos_lable posLable)
		{
			return Cache<T>.contains(_fileId) && Cache<T>[_fileId].contains(posLable);
		}

		template <typename T>
		void Add(pos_lable posLable, shared_ptr<T> object)
		{
			if (!Cache<T>.contains(_fileId))
			{
				_unloader = [fileId = _fileId, previousUnloader = move(_unloader)]()
				{
					previousUnloader();
					Cache<T>.erase(fileId);
				};
			}

			Cache<T>.insert({ _fileId,
			{ 
				{ posLable, object },
			}});
		}

		template <typename T>
		void Remove(pos_lable posLable)
		{
			Cache<T>[_fileId].erase(posLable);
		}

		template <typename T>
		shared_ptr<T> Read(pos_lable posLable)
		{
			return Cache<T>[_fileId][posLable];
		}
	};

	template <typename T>
	map<FileCache::id_int, map<pos_lable, shared_ptr<T>>> FileCache::Cache = {};
}