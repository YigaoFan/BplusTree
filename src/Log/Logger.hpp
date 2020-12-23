#pragma once
#include <tuple>
#include <string>
#include <sstream>
#include <type_traits>
#include "../Basic/TypeTrait.hpp"
#include "Curry.hpp"
#include "FormatMap.hpp"
#include "FormatParser.hpp"

namespace Log
{
	using Basic::FuncTraits;
	using ::std::forward;
	using ::std::is_same_v;
	using ::std::remove_reference_t;
	using ::std::string;
	using ::std::stringstream;
	using ::std::tie;
	using ::std::tuple;
	using ::std::tuple_cat;
	using ::std::tuple_size_v;

	// 分离 hpp 和 cpp
	// 每天存档前一日的 log 文件，触发 log 操作的时候检查下，或者能设置定时回调吗？设个定时任务
	class Logger
	{
	private:
		// 可以设置向控制台和文件输出，输出后清空
		stringstream _strStream { stringstream::out | stringstream::app };

	public:
		Logger()
		{

		}

		// 用前三个作为基础 log 内容
		template <size_t BasicInfoLimitCount, typename CurryedAccessLogger, typename CurrentBasicInfoTuple>
		struct SubLogger;
		template <typename CurrentBasicInfoTuple>
		struct NonAccessLogSubLogger;

		template <typename... Infos>
		void WriteAccessLog(Infos... infos)
		{
			// TODO
		}

		template <char... FormatChars>
		auto GenerateSubLogger(integer_sequence<char, FormatChars...> format)
		{
			auto typeList = ParseFormat(format);
			using TypeList = decltype(typeList);
			constexpr int size = Length<TypeList>::Result;
			auto idxs = make_index_sequence<size>();
			constexpr size_t basicInfoLimitCount = 3;
			return MakeSubLogger<basicInfoLimitCount>(GetCurryedAccessLog(typeList, idxs), this, tuple());
		}
		
		// below with some basic info
		void Info(string message)
		{

		}

		void Warn(string message)
		{

		}

		template <typename Exception>
		void Warn(string message, Exception const& exception)
		{

		}

		template <typename Exception>
		void Error(string message, Exception const& exception)
		{
			
		}
	
	private:
		template <typename TypeList, size_t... Idxs>
		auto GetCurryedAccessLog(TypeList, index_sequence<Idxs...>)
		{
			auto funcAddr = &Logger::WriteAccessLog<typename Get<TypeList, Idxs>::Result...>;
			return Curry(funcAddr)(this);
		}

		template <size_t BasicInfoLimitCount, typename CurryedAccessLogger, typename CurrentBasicInfoTuple>
		static auto MakeSubLogger(CurryedAccessLogger curryedAccessLog, Logger *parentLogger, CurrentBasicInfoTuple infos)
		{
			return SubLogger<BasicInfoLimitCount, CurryedAccessLogger, CurrentBasicInfoTuple>(
				move(curryedAccessLog), move(parentLogger), move(infos));
		}
	};

	template <size_t BasicInfoLimitCount, typename CurryedAccessLogger, typename CurrentBasicInfoTuple>
	struct Logger::SubLogger : private NonAccessLogSubLogger<CurrentBasicInfoTuple>
	{
		using Base = NonAccessLogSubLogger<CurrentBasicInfoTuple>;
		CurryedAccessLogger CurryedAccessLog;

		// 这里的构造函数推导要怎么用？可以直接用吗而不完全指定类型的模板参数，可见当时委员会的人也可能比较困惑
		// 所以后来才支持这个特性
		SubLogger(CurryedAccessLogger curryedAccessLog, Logger *parentLogger, CurrentBasicInfoTuple infos)
			: Base(move(infos), parentLogger), CurryedAccessLog(move(curryedAccessLog))
		{ }
		// move constructor and set ParentLogger to nullptr

		using NextInfo = typename FuncTraits<decltype(&CurryedAccessLogger::operator())>::template Arg<0>::Type;
		auto BornNewWith(NextInfo info)
		{
			auto parent = this->ParentLogger;

			if constexpr (not is_same_v<void, decltype(CurryedAccessLog(forward<NextInfo>(info)))>)
			{
				auto newerAccessLog = CurryedAccessLog(forward<NextInfo>(info));

				if constexpr (tuple_size_v<decltype(this->BasicInfo)> < BasicInfoLimitCount)
				{
					auto newBasicInfo = tuple_cat(this->BasicInfo, tuple(remove_reference_t<NextInfo>(info)));
					return SubLogger<BasicInfoLimitCount, decltype(newerAccessLog), decltype(newBasicInfo)>{
						move(newerAccessLog), parent, move(newBasicInfo) };
				}
				else
				{
					return SubLogger<BasicInfoLimitCount, decltype(newerAccessLog), CurrentBasicInfoTuple>{
						move(newerAccessLog), parent, move(this->BasicInfo) };
				}
			}
			else
			{
				CurryedAccessLog(forward<NextInfo>(info));

				if constexpr (tuple_size_v<decltype(this->BasicInfo)> < BasicInfoLimitCount)
				{
					auto newBasicInfo = tuple_cat(this->BasicInfo, tuple(remove_reference_t<NextInfo>(info)));
					return NonAccessLogSubLogger(move(newBasicInfo), parent);
				}
				else
				{
					return NonAccessLogSubLogger(move(this->BasicInfo), parent);
				}
			}
		}

		using Base::Info;
		using Base::Warn;
		using Base::Error;
	};

	template <typename CurrentBasicInfoTuple>
	struct Logger::NonAccessLogSubLogger
	{
		CurrentBasicInfoTuple BasicInfo; // 作为后来 log 的前缀
		Logger* ParentLogger;

		// 原来构造函数的类模板参数推导可以这样用，结合调用的地方一起看
		NonAccessLogSubLogger(CurrentBasicInfoTuple infos, Logger* parentLogger)
			: BasicInfo(move(infos)), ParentLogger(parentLogger)
		{ }
		
		void Info(string message)
		{
		}

		void Warn(string message)
		{
		}

		template <typename Exception>
		void Warn(string message, Exception const& exception)
		{
		}

		void Error(string message)
		{
		}

		template <typename Exception>
		void Error(string message, Exception const& exception)
		{
		}
	};

	// https://ctrpeach.io/posts/cpp20-string-literal-template-parameters/

	Logger MakeLogger()
	{
		return Logger();
	}
}
