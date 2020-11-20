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

	/// 不支持继承体系下的动态类型，所以需要使用者注意存取的类型
	/// 一个 File 仅有一个 FileCache
	class FileCache
	{
	public:
		using id_int = unsigned int;
	private:
		template <typename T>
		/// file id, pos label, object
		static map<id_int, map<pos_label, shared_ptr<T>>> Cache;

		id_int _fileId;
		function<void()> _unloader = []() {};
	public:
		FileCache(id_int fileId);
		~FileCache();

		template <typename T>
		bool Cached(pos_label posLabel)
		{
			return Cache<T>.contains(_fileId) && Cache<T>[_fileId].contains(posLabel);
		}

		template <typename T>
		void Add(pos_label posLabel, shared_ptr<T> object)
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
				{ posLabel, object },
			}});
		}

		template <typename T>
		void Remove(pos_label posLabel)
		{
			Cache<T>[_fileId].erase(posLabel);
		}

		template <typename T>
		shared_ptr<T> Read(pos_label posLabel)
		{
			return Cache<T>[_fileId][posLabel];
		}
	};

	template <typename T>
	map<FileCache::id_int, map<pos_label, shared_ptr<T>>> FileCache::Cache = {};
}