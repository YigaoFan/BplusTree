#pragma once
#include <map>
#include <memory>
#include <vector>
#include <variant>
#include <functional>
#include "StaticConfig.hpp"

namespace FuncLib::Store
{
	using ::std::function;
	using ::std::get;
	using ::std::holds_alternative;
	using ::std::map;
	using ::std::move;
	using ::std::shared_ptr;
	using ::std::variant;
	using ::std::vector;

	/// 不支持继承体系下的动态类型，所以需要使用者注意存取的类型
	/// 一个 File 仅有一个 FileCache
	class FileCache
	{
	public:
		using id_int = unsigned int;
	private:
		template <typename T>
		using SettersOf = vector<function<void(T*)>>;
		template <typename T>
		/// file id, pos label, setters or object
		inline static map<id_int, map<pos_label, variant<SettersOf<T>, shared_ptr<T>>>> Cache = {};

		id_int _fileId;
		function<void()> _unloader = []() {};
	public:
		FileCache(id_int fileId);
		~FileCache();

		template <typename T>
		bool Cached(pos_label posLabel) const
		{
			return Cache<T>.contains(_fileId) 
				and Cache<T>[_fileId].contains(posLabel)
				and holds_alternative<shared_ptr<T>>(Cache<T>[_fileId][posLabel]);
		}

		template <typename T>
		void RegisterSetter(pos_label posLabel, function<void(T*)> setter)
		{
			if (holds_alternative<SettersOf<T>>(Cache<T>[_fileId][posLabel]))
			{
				get<SettersOf<T>>(Cache<T>[_fileId][posLabel]).push_back(move(setter));
			}
			else
			{
				auto& obj = get<shared_ptr<T>>(Cache<T>[_fileId][posLabel]);
				setter(obj.get());
			}
			// 如果析构的时候仍有 setters，那就需要读一下，读一下，以及设置下就有个顺序的问题，采用队列吧 TODO
			// 或者先报异常不是处理吧
		}

		template <typename T>
		bool HasSetter(pos_label posLabel)
		{
			if (Cache<T>[_fileId].contains(posLabel))
			{
				if (holds_alternative<SettersOf<T>>(Cache<T>[_fileId][posLabel]))
				{
					return not get<SettersOf<T>>(Cache<T>[_fileId][posLabel]).empty();
				}
			}

			return false;
		}

		/// Precondition: HasSetter return true
		template <typename T>
		void Set(pos_label label, T* object)
		{
			auto& v = Cache<T>[_fileId][label];
			auto& setters = std::get<SettersOf<T>>(v);
			for (auto& s : setters)
			{
				s(object);
			}
		}

		template <typename T>
		void Add(pos_label posLabel, shared_ptr<T> object)
		{
			if (not Cache<T>.contains(_fileId))
			{
				_unloader = [fileId = _fileId, previousUnloader = move(_unloader)]()
				{
					previousUnloader();
					Cache<T>.erase(fileId);
				};
			}

			if (HasSetter<T>(posLabel))
			{
				Set<T>(posLabel, object.get());
			}

			Cache<T>[_fileId][posLabel] = object;
		}

		template <typename T>
		void Remove(pos_label posLabel)
		{
			Cache<T>[_fileId].erase(posLabel);
		}

		template <typename T>
		shared_ptr<T> Read(pos_label posLabel)
		{
			return get<shared_ptr<T>>(Cache<T>[_fileId][posLabel]);
		}
	};
}