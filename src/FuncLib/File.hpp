#pragma once
#include <filesystem>
#include <fstream>
#include <vector>
#include <array>
#include <cstddef>
#include <memory>
#include <utility>
#include <map>
#include <set>
#include <functional>
#include "FileCache.hpp"

namespace FuncLib
{
	using ::std::array;
	using ::std::byte;
	using ::std::enable_shared_from_this;
	using ::std::function;
	using ::std::ifstream;
	using ::std::make_shared;
	using ::std::map;
	using ::std::move;
	using ::std::owner_less;
	using ::std::pair;
	using ::std::set;
	using ::std::shared_ptr;
	using ::std::size_t;
	using ::std::vector;
	using ::std::weak_ptr;
	using ::std::filesystem::path;

	constexpr uint32_t DiskBlockSize = 4096; // Depend on different OS setting
	using pos_int = size_t;

	class File : public enable_shared_from_this<File>
	{
	private:
		static set<weak_ptr<File>, owner_less<weak_ptr<File>>> Files;

		path _filename;
		FileCache _cache;
		function<void()> _unloader;
		pos_int _currentPos; // need to have data in file to save allocated information
	public:
		static shared_ptr<File> GetFile(path const& filename)
		{
			for (auto& weakFile : Files)
			{
				auto f = weakFile.lock();
				if (f->_filename == filename)
				{
					return f;
				}
			}

			auto f = make_shared<File>(filename);
			Files.insert(f);
			f->_unloader = [f = weak_ptr<File>(f)]()
			{
				Files.erase(f);
			};
			return f;
		}

		// only for make_shared use in File
		File(path filename, pos_int startPos = 0);

		void Flush();

		// template <auto N>
		// array<byte, N> Read(pos_int start)
		// {
		// 	Flush();
		// 	ifstream fs(_filename, ifstream::binary);
		// 	fs.seekg(start);

		// 	array<byte, N> mem;

		// 	if (fs.is_open())
		// 	{
		// 		fs.read(reinterpret_cast<char *>(&mem[0]), N);
		// 	}

		// 	return mem;
		// }

		// vector<byte> Read(pos_int start, size_t size);

		template <typename T>
		shared_ptr<T> Read(pos_int start)
		{
			if (_cache.HasRead<T>(start))
			{
				return _cache.Read<T>(start);
			}
			else
			{
				auto obj = make_shared<T>(Read<T>(start));
				_cache.Add<T>(start, obj);
				return obj;
			}
		}

		// used in string like type
		template <typename T>
		shared_ptr<T> Read(pos_int start, size_t size, function<T(vector<byte>)> converter)
		{
			if (_cache.HasRead<T>(start))
			{
				return _cache.Read<T>(start);
			}
			else
			{
				auto obj = make_shared<T>(converter(Read(start, size)));
				_cache.Add<T>(start, obj);
				return _cache.Read<T>(start);
			}
		}
		
		template <typename T, typename Bytes>
		shared_ptr<T> New(Bytes bytes)// should be a type which has iterator
		{
			auto pos = 0;
			auto p = shared_ptr<T>(new T(t), [](auto p)
			{
				Write(pos, bytes);
				delete p;
			});
			_cache.Add(pos, p);
			return p;
		}

		// TODO how to reallocate all content
		template <typename T>
		void Delete(shared_ptr<T> object)
		{
			
		}

		// add a function to rewrite all object in disk memory to make reduce memory fragmentation
		// need to save allocate info in file
		// void Deallocate(pos_int pos, size_t size)
		// {
			// Flush();
			// TODO help doc: https://zhuanlan.zhihu.com/p/29415507
		// }

		void WriteFromHead()
		{
			// for what?
		}

		~File();
	private:
		template <typename T>
		T Read(pos_int start)
		{
			ifstream fs(_filename, ifstream::binary);
			fs.seekg(start);

			byte mem[sizeof(T)];
			if (fs.is_open())
			{
				fs.read(reinterpret_cast<char *>(&mem[0]), sizeof(T));
			}

			T* p = reinterpret_cast<T *>(&mem[0]);
			return move(*p);
		}

		template <typename T>
		void Write(pos_int start, vector<byte> data)
		{
			// ...
		}

		template <typename T, auto Size>
		void Write(pos_int start, array<byte, Size> data)
		{
			// ...
		}
	};

}