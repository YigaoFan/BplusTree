#pragma once
#include <tuple>
#include <chrono>
#include <string>
#include <sstream>
#include <string_view>
#include <type_traits>
#include "../Basic/TypeTrait.hpp"
#include "Curry.hpp"
#include "FormatMap.hpp"
#include "FormatParser.hpp"

namespace std
{
	string to_string(string s)
	{
		return s;
	}

	string to_string(Log::ResultStatus s)
	{
#define nameof(VAR) #VAR
		switch (s)
		{
		case Log::ResultStatus::Complete:
			return nameof(Complete);
		
		case Log::ResultStatus::Failed:
			return nameof(Failed);

		case Log::ResultStatus::Undefine:
			return nameof(Undefine);
		}
#undef nameof
	}
}

namespace Log
{
	using Basic::FuncTraits;
	using ::std::forward;
	using ::std::index_sequence;
	using ::std::is_same_v;
	using ::std::make_index_sequence;
	using ::std::remove_reference_t;
	using ::std::string;
	using ::std::string_view;
	using ::std::stringstream;
	using ::std::tie;
	using ::std::tuple;
	using ::std::tuple_cat;
	using ::std::tuple_size_v;

	// 分离 hpp 和 cpp
	// 每天存档前一日的 log 文件，触发 log 操作的时候检查下，或者能设置定时回调吗？设个定时任务
	// 要加锁吗？
	class Logger
	{
	private:
		// 可以设置向控制台和文件输出，输出后清空
		stringstream _strStream { stringstream::out | stringstream::app };

	public:
		Logger()
		{

		}

		template <size_t BasicInfoLimitCount, typename CurryedAccessLogger, typename CurrentBasicInfoTuple>
		struct AccessLogSubLogger;
		template <typename CurrentBasicInfoTuple>
		struct NonAccessLogSubLogger;

		template <typename... Infos>
		void WriteAccessLog(Infos... infos)
		{
			WriteLine(move(std::to_string(infos))...);
		}

		template <char... FormatChars>
		auto GenerateSubLogger(integer_sequence<char, FormatChars...> format)
		{
			auto typeList = ParseFormat(format);
			using TypeList = decltype(typeList);
			constexpr int size = Length<TypeList>::Result;
			auto idxs = make_index_sequence<size>();
			// 用前三个作为基础 log 内容
			constexpr size_t basicInfoLimitCount = 3;
			return MakeSubLogger<basicInfoLimitCount>(GetCurryedAccessLog(typeList, idxs), this, tuple());
		}
		
		void Info(string message)
		{
			WriteLine(message);
		}

		void Warn(string message)
		{
			WriteLine(message);
		}

		template <typename Exception>
		void Warn(string message, Exception const& exception)
		{
			WriteLine(message, exception.what());
		}

		template <typename Exception>
		void Error(string message, Exception const& exception)
		{
			WriteLine(message, exception.what());
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
			return AccessLogSubLogger<BasicInfoLimitCount, CurryedAccessLogger, CurrentBasicInfoTuple>(
				move(curryedAccessLog), move(parentLogger), move(infos));
		}

		/// Message is the type which is supported by stringstream operator<<
		template <bool InnerCall = false, typename Message, typename... Messages>
		void WriteLine(Message message, Messages... messages)
		{
			if constexpr (not InnerCall)
			{
				auto now = std::chrono::system_clock::now();
				auto clock = std::chrono::system_clock::to_time_t(now);
				_strStream << std::put_time(std::localtime(&clock), "%F %T") << ' ';
			}

			_strStream << message;
			if constexpr (sizeof... (messages) > 0)
			{
				_strStream << ' ';
				WriteLine<true>(forward<Messages>(messages)...);
			}
			else
			{
				_strStream << std::endl;
			}
		}
	};

	template <size_t BasicInfoLimitCount, typename CurryedAccessLogger, typename CurrentBasicInfoTuple>
	struct Logger::AccessLogSubLogger : private NonAccessLogSubLogger<CurrentBasicInfoTuple>
	{
		using Base = NonAccessLogSubLogger<CurrentBasicInfoTuple>;
		CurryedAccessLogger CurryedAccessLog;

		// 这里的构造函数推导要怎么用？可以直接用吗而不完全指定类型的模板参数，可见当时委员会的人也可能比较困惑
		// 所以后来才支持这个特性
		AccessLogSubLogger(CurryedAccessLogger curryedAccessLog, Logger *parentLogger, CurrentBasicInfoTuple infos)
			: Base(move(infos), parentLogger), CurryedAccessLog(move(curryedAccessLog))
		{ }

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
					return AccessLogSubLogger<BasicInfoLimitCount, decltype(newerAccessLog), decltype(newBasicInfo)>{
						move(newerAccessLog), parent, move(newBasicInfo) };
				}
				else
				{
					return AccessLogSubLogger<BasicInfoLimitCount, decltype(newerAccessLog), CurrentBasicInfoTuple>{
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

		~AccessLogSubLogger()
		{
			if (tuple_size_v<decltype(this->BasicInfo)> < BasicInfoLimitCount)
			{
				if (auto ePtr = std::current_exception(); ePtr != nullptr)
				{
					try
					{
						std::rethrow_exception(ePtr);
					}
					catch (std::exception const& e)
					{
						this->ParentLogger->Error(this->GetBasicInfoLogPrefix() + " Access terminated:", e);
					}
				}
			}
		}
	};

	template <typename CurrentBasicInfoTuple>
	struct Logger::NonAccessLogSubLogger
	{
		CurrentBasicInfoTuple BasicInfo;
		Logger* ParentLogger;

		// 原来构造函数的类模板参数推导可以这样用，结合调用的地方一起看
		NonAccessLogSubLogger(CurrentBasicInfoTuple infos, Logger* parentLogger)
			: BasicInfo(move(infos)), ParentLogger(parentLogger)
		{ }
		
		void Info(string message)
		{
			auto m = GetBasicInfoLogPrefix() + ' ' + message;
			ParentLogger->Info(move(m));
		}

		void Warn(string message)
		{
			auto m = GetBasicInfoLogPrefix() + ' ' + message;
			ParentLogger->Warn(move(m));
		}

		template <typename Exception>
		void Warn(string message, Exception const& exception)
		{
			auto m = GetBasicInfoLogPrefix() + ' ' + message;
			ParentLogger->Warn(move(m), exception);
		}

		void Error(string message)
		{
			auto m = GetBasicInfoLogPrefix() + ' ' + message;
			ParentLogger->Warn(move(m));
		}

		template <typename Exception>
		void Error(string message, Exception const& exception)
		{
			auto m = GetBasicInfoLogPrefix() + ' ' + message;
			ParentLogger->Warn(move(m), exception);
		}

	protected:
		string GetBasicInfoLogPrefix() const
		{
			auto generate = [&]<size_t... Idxs>(index_sequence<Idxs...>)
			{
				return (string() + ... + std::to_string(get<Idxs>(BasicInfo)));
			};

			auto idxs = make_index_sequence<tuple_size_v<CurrentBasicInfoTuple>>();
			return generate(idxs);
		}
	};

	// https://ctrpeach.io/posts/cpp20-string-literal-template-parameters/

	Logger MakeLogger()
	{
		return Logger();
	}
}
