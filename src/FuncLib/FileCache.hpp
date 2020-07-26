#include <vector>
#include <memory>
#include <utility>
#include <map>
#include <functional>

namespace FuncLib
{
	using ::std::function;
	using ::std::make_pair;
	using ::std::map;
	using ::std::pair;
	using ::std::shared_ptr;
	using ::std::vector;

	class File;
	
	class FileCache
	{
	private:
		template <typename T>
		static map<shared_ptr<File>, map<size_t, pair<shared_ptr<T>, vector<function<void(T*)>>>>> Cache;
		static map<shared_ptr<File>, function<void()>> Unloaders;

	public:
		FileCache() // shared_ptr<File> file
		{
			// Use this constructor to obtain a access key(token)
		}

		template <typename T>
		bool HasRead(size_t offset)
		{
			shared_ptr<File> file;
			// maybe should compare by File operator==
			return Cache<T>.contains(file) && Cache<T>[file].contains(offset);
		}

		template <typename T>
		shared_ptr<T> Add(size_t offset, shared_ptr<T> object)
		{
			shared_ptr<File> file;

			Cache<T>[file][offset] = make_pair<shared_ptr<T>, vector<function<void(T*)>>>(object, { });

			auto newUnloader = [&target = Cache<T>[file][offset]]()
			{
				for (auto& setter : target.second)
				{
					setter(target.first.get());
				}
			};

			if (Unloaders.contains(file))
			{
				auto preUnloader = Unloaders[file];
				newUnloader = [= move(preUnloader), newPart = move(newUnloader)]()
				{
					preUnloader();
					newPart();
				};
			}

			Unloaders[file] = move(newUnloader);
		}
		
		~FileCache()
		{
			// Invoke all unloaders
		}
	}; 
}
