#pragma once
#include <filesystem>
#include <fstream>
#include <vector>
#include <array>
#include <cstddef>
#include <memory>
#include <utility>
#include <set>
#include <functional>
#include "FileCache.hpp"
#include "../Basic/Exception.hpp"

namespace FuncLib
{
	using ::std::array;
	using ::std::byte;
	using ::std::enable_shared_from_this;
	using ::std::forward;
	using ::std::function;
	using ::std::ifstream;
	using ::std::make_shared;
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

		shared_ptr<path> _filename;// 变成 shared_ptr
		FileCache _cache;
		function<void()> _unloader;
		pos_int _currentPos; // need to have data in file to save allocated information
	public:
		static shared_ptr<File> GetFile(path const& filename);
		File(path const& filename, pos_int startPos = 0); // for make_shared use in File class only
		void Flush();

		// set the deleter of object
		template <typename T>
		shared_ptr<T> Read(pos_int start)
		{
			if (_cache.HasRead<T>(start))
			{
				return _cache.Read<T>(start);
			}
			else
			{
				auto obj = make_shared<T>(Read<T>(start));// TODO use Package
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
				auto obj = make_shared<T>(converter(Read(start, size))); // TODO use Package
				_cache.Add<T>(start, obj);
				return _cache.Read<T>(start);
			}
		}
		
		template <typename T, typename Bytes>
		shared_ptr<T> New(T&& t)
		{
			auto p = Package(new T(forward(t)));
			return p;
		}

		template <typename T>
		void Delete(shared_ptr<T> object)
		{
			using ::Basic::InvalidOperationException;
			constexpr auto minRefCount = 3;
			if (object.use_count() == minRefCount)
			{
				_cache.Remove(object);
			}
			else
			{
				throw InvalidOperationException
					("object to delete still has some place using");
			}
			
			// delete cache and other place not ref this
		}

		void ReallocateContent()
		{
			// if want to reallocate all disk content, attention the DiskPtr config
		}

		~File();
	private:
		// not suggest use
		template <typename T>
		static T Read(path const& filename, pos_int start)
		{
			ifstream fs(filename, ifstream::binary);
			fs.seekg(start);

			byte mem[sizeof(T)];
			if (fs.is_open())
			{
				fs.read(reinterpret_cast<char *>(&mem[0]), sizeof(T));
			}

			T* p = reinterpret_cast<T *>(&mem[0]);
			return move(*p);
		}

		vector<byte> Read(pos_int start, size_t size)
		{
			ifstream fs(_filename, ifstream::binary);
			fs.seekg(start);

			vector<byte> mem(size);

			if (fs.is_open())
			{
				fs.read(reinterpret_cast<char *>(&mem[0]), size);
			}

			return mem;
		}

		template <typename T, typename FowardIter>
		static void Write(path const& filename, pos_int start, ForwardIter begin, ForwardIter end)
		{
			ofstream fs(filename, ofstream::binary);
			fs.seekp(start);
			fs.write(begin, end - begin);
		}

		shared_ptr<T> PackageThenAddToCache(T* ptr)
		{
			auto p = shared_ptr<T>(ptr, [filename = _filename](auto p)
			{
				// convert to bytes
				// use a class object package these store info and operation
				// Manager class can do this?
				auto bytes;
				Write(*filename, pos, bytes.begin(), bytes.end());
				delete p;
			});
			_cache.Add<T>(pos, p);

			return p;
		}
	};
}