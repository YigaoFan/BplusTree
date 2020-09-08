#pragma once
#include <filesystem>
#include <fstream>
#include <vector>
#include <memory>
#include <set>
#include <functional>
#include "FileCache.hpp"
#include "StaticConfig.hpp"
#include "IInsidePositionOwner.hpp"
#include "../ByteConverter.hpp"
#include "../Basic/Exception.hpp"

namespace FuncLib::Store
{
	using ::std::byte;
	using ::std::enable_shared_from_this;
	using ::std::forward;
	using ::std::function;
	using ::std::ifstream;
	using ::std::make_shared;
	using ::std::move;
	using ::std::owner_less;
	using ::std::set;
	using ::std::shared_ptr;
	using ::std::size_t;
	using ::std::vector;
	using ::std::weak_ptr;
	using ::std::filesystem::path;

	/// 一个路径仅有一个 File 对象
	class File : public enable_shared_from_this<File>
	{
	private:
		static set<weak_ptr<File>, owner_less<weak_ptr<File>>> Files;

		shared_ptr<path> _filename;
		FileCache _cache;
		function<void()> _unloader = []() {};
		pos_int _currentPos;
	public:
		static shared_ptr<File> GetFile(path const& filename);
		File(path const& filename, pos_int startPos = 0); // for make_shared use in File class only
		void Flush();

		template <typename T>
		shared_ptr<T> Read(shared_ptr<IInsidePositionOwner> posOwner)
		{
			auto addr = posOwner->Addr();
			if (_cache.HasRead<T>(addr))
			{
				return _cache.Read<T>(addr);
			}
			else
			{
				return PackageThenAddToCache(new T(Read<T>(*_filename, addr)), posOwner);
			}
		}

		/// used in string like type
		template <typename T>
		shared_ptr<T> Read(shared_ptr<IInsidePositionOwner> posOwner, function<T(vector<byte>)> converter)
		{
			auto addr = posOwner->Addr();
			if (_cache.HasRead<T>(addr))
			{
				return _cache.Read<T>(addr);
			}
			else
			{
				return PackageThenAddToCache(new T(converter(Read(addr, posOwner->RequiredSize()))), posOwner);
			}
		}
		
		template <typename T>
		shared_ptr<T> New(T&& t)
		{
			auto posOwner = make_shared<DiskPos<T>>(shared_from_this(), _currentPos);
			_currentPos += sizeof(T);
			auto p = PackageThenAddToCache(new T(forward(t)), posOwner);
			return p;
		}

		template <typename T>
		void Delete(shared_ptr<T> object)
		{
			using ::Basic::InvalidOperationException;
			constexpr auto minRefCount = 3;
			// if (object.use_count() == minRefCount)
			// {
				// log object.use_count();
				_cache.Remove(object);
			// }
			// else
			// {
			// 	throw InvalidOperationException
			// 		("object to delete still has some place using");
			// }
		}

		/// caller should ensure wake all root element, j
		/// ust like a btree can wake all inner elements, but not other btree.
		void ReallocateContent();

		~File();
	private:
		static vector<byte> Read(path const& filename, pos_int start, size_t size);

		template <typename T, typename FowardIter>
		static void Write(path const& filename, pos_int start, ForwardIter begin, ForwardIter end)
		{
			ofstream fs(filename, ofstream::binary);
			fs.seekp(start);
			fs.write((char *)begin, end - begin);
		}

		/// 使用下面这个的场景是：MiddleNode 包含 Elements 的时候，就不能返回 shared_ptr 了
		/// 而是要用这个了，由 MiddleNode 级别来提供缓存的功能，也就是说要读 Elements 只能通过
		/// MiddleNode 来，这个 Elements 对外是隐形的。如果直接读的话，就错了。
		/// 这个函数的返回值不提供返回值缓存功能
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

			T *p = reinterpret_cast<T *>(&mem[0]);
			return move(*p);
		}

		template <typename T>
		shared_ptr<T> PackageThenAddToCache(T* ptr, shared_ptr<IInsidePositionOwner> posOwner)
		{
			auto p = shared_ptr<T>(ptr, [file = _filename, =posOwner](auto p)
			{
				// convert to bytes
				vector<byte> bytes;
				Write(*file, posOwner->Addr(), bytes.begin(), bytes.end());
				delete p;
			});
			_cache.Add<T>(posOwner, p);

			return p;
		}
	};
}