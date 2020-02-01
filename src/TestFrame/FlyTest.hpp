#pragma once

#include <functional>
#include <vector>
#include <string>
#include <tuple>
#include <iostream>
#include <utility>
#include <cstring>
#include "Exception.hpp"
// for unknown exception handle
#ifdef __clang__
#include "cxxabi.h"
#define DEPEND_ON_COMPILER_SUPPORT(SUPPORT_DO, NOT_SUPPORT_DO) SUPPORT_DO
#elif __GNUC__
#include "cxxabi.h"
#define DEPEND_ON_COMPILEER_SUPPORT(SUPPORT_DO, NOT_SUPPORT_DO) SUPPORT_DO
#else
#define DEPEND_ON_COMPILER_SUPPORT(SUPPORT_DO, NOT_SUPPORT_DO) NOT_SUPPORT_DO
#endif

namespace
{
	using ::std::pair;
	using ::std::make_pair;
	using ::std::string;
	using ::std::vector;
	using ::std::tuple;
	using ::std::make_tuple;
	using ::std::cout;
	using ::std::endl;
	using ::std::ostream;
	using ::std::flush;
	using ::std::function;
	using ::std::runtime_error;
	using ::std::size_t;
	using ::std::move;
	using ::Exception::AssertionFailure;

#define PRIMITIVE_CAT(A, B) A##B
#define CAT(A, B) PRIMITIVE_CAT(A, B)

	class Condition;

	struct Info
	{
	public:
		string fileName;
		size_t line;
		string description;

		Info() = default;

		Info(string fileName, size_t line, string description)
			: fileName(move(fileName)), line(line), description(move(description))
		{}
	};

	class SectionRouteTrack
	{
	private:
		vector<Info> _route{};
		int32_t _currentSectionIndex = -1;
	public:
		SectionRouteTrack() = default;

		SectionRouteTrack& pushBack(Info info)
		{
			_route.emplace_back(move(info));
			_currentSectionIndex = (int32_t)(_route.size() - 1);
			return *this;
		}

		void moveBack()
		{
			--_currentSectionIndex;
		}

		void log(int8_t initialIndent = 0, ostream& out = cout) const
		{
			if (_currentSectionIndex < 0)
			{
				throw runtime_error("Wrong invoke log, only need to call log when SECTION doesn't exit normally");
			}

			for (auto i = 0; i < _route.size(); ++i)
			{
				showNSpace(initialIndent, out);
				if (i == _currentSectionIndex)
				{
					out << "-> ";
				}

				showSectionInfo(_route[i], out) << endl;
				++initialIndent;
			}
		}

	private:
		static ostream& showNSpace(uint32_t num, ostream& out)
		{
			for (decltype(num) i = 0; i < num; ++i)
			{
				out << ' ';
			}

			return out.flush();
		}

		static ostream& showSectionInfo(Info const& sectionInfo, ostream& out)
		{
			return (out
				<< sectionInfo.fileName << ":"
				<< sectionInfo.line << ": "
				<< sectionInfo.description).flush();
		}
	};

	class Section
	{
		// Section don't have the data detail rely on Condition class, but Condition do.
		// So we can pass a condition as parameter in TESTCASE not reference without error
		friend class Condition;
	private:
		bool _selfDone{ false };
		vector<Section*> _subSections{};
		Info _info;

	public:
		Section(Condition& condition, Info info);

		explicit Section(Info info) : _info(move(info)) {}

		bool shouldExecute()
		{
			return !_selfDone;
		}

		void markDone()
		{
			_selfDone = true;
		}

		Info info() const
		{
			return _info;
		}
	};

	class Condition
	{
	private:
		bool& _state;
		bool _shouldExecute;
	public:
		Section& correspondSection;
		SectionRouteTrack& track;

		Condition(Section& section, bool& state, SectionRouteTrack& track)
			: _state(state), _shouldExecute(true), correspondSection(section), track(track)
		{ }

		operator bool()
		{
			// _state is true, means this run end
			_shouldExecute = _state ? false : correspondSection.shouldExecute();

			if (_shouldExecute)
			{
				track.pushBack(correspondSection.info());
			}

			return _shouldExecute;
		}

		~Condition()
		{
			if (_shouldExecute)
			{
				if (!_state)
				{
					correspondSection.markDone();
					_state = true;
				}

				if (!uncaughtException())
				{
					track.moveBack();
				}
			}
		}

	private:
		static bool uncaughtException()
		{
#if __cplusplus >= 201703L
			return ::std::uncaught_exceptions() > 0;
#elif __cplusplus >= 201103L
			return ::std::uncaught_exception();
#else
// If you use VS2017 encounter this problem that should not occur
//, you could add an addtional command line arg in [Project Properties]->[C/C++]->[Command Line]->[Additional Options]: /Zc:__cplusplus
#error This program requires C++ version at least C++11, please update your compiler setting
#endif
		}
	};

	Section::Section(Condition& condition, Info info)
		: _info(move(info))
	{
		// register sub-section
		condition.correspondSection._subSections.emplace_back(this);
	}

	using TestCaseFunction = function<void(Condition&&, bool&, SectionRouteTrack&, uint16_t&)>;
	vector<pair<Info, TestCaseFunction>> _tests_{};

	class RegisterTestCase
	{
	public:
		RegisterTestCase(pair<Info, TestCaseFunction> infoTestCasePair)
		{
			_tests_.emplace_back(move(infoTestCasePair));
		}
	};

	void allTest()
	{
		auto& out = cout;
		auto log = [&](string const& exceptionTypeName, string const& exceptionContent, SectionRouteTrack& track)
		{
			out
				<< "\n"
				<< "Caught exception of type " << '\'' << exceptionTypeName << '\'' << endl
				<< exceptionContent << endl
				<< "Testcase state:" << endl;
			track.log(0, out);
		};

		for (auto& t : _tests_)
		{
			Section testCase(t.first);
			uint32_t successCount = 0;
			uint32_t failureCount = 0;
			out << "Testcase: " << testCase.info().description << endl;

			while (testCase.shouldExecute())
			{
				auto onceState = false;
				SectionRouteTrack sectionTrack;
				sectionTrack.pushBack(testCase.info());

				try
				{
					t.second(Condition(testCase, onceState, sectionTrack), onceState, sectionTrack,
						successCount);
				}
				catch (::std::exception const& e)
				{
					++failureCount;
					log(string{ typeid(e).name() }, string{ e.what() }, sectionTrack);
				}
				catch (int i)
				{
					++failureCount;
					log("int", ::std::to_string(i), sectionTrack);
				}
				catch (...)
				{
					++failureCount;
					log(DEPEND_ON_COMPILER_SUPPORT(__cxxabiv1::__cxa_current_exception_type()->name(), "Unknown type"),
						"",
						sectionTrack);
				}
				if (!onceState)
				{ 
					// meas onceState not be changed
					break;
				}
			}

			out
				<< '\n'
				<< "Result: "
				<< failureCount << " Failed, "
				<< successCount << " Passed\n" << endl;
		}
	}

	char const* getFileName(char const* str)
	{
		return ::std::strrchr(str, '/') + 1;
	}



#define TESTCASE(DESCRIPTION)                                                                                                                                          \
    void CAT(testcase, __LINE__) (Condition&& , bool& , SectionRouteTrack&, uint16_t&);                                                                                \
    RegisterTestCase CAT(registerTestcase, __LINE__) = make_pair<Info, TestCaseFunction>(Info(getFileName(__FILE__), __LINE__, DESCRIPTION), CAT(testcase, __LINE__)); \
    void CAT(testcase, __LINE__) (Condition&& condition, bool& onceState, SectionRouteTrack& track, uint16_t& successCount)

#define SECTION(DESCRIPTION) Section CAT(section, __LINE__) { condition, Info(getFileName(__FILE__), __LINE__, DESCRIPTION) }; \
    if (Condition condition{ CAT(section, __LINE__) , onceState, track })

#define ASSERT(EXP)                                                                            \
    do {                                                                                       \
        if (!(EXP)) {                                                                          \
            throw AssertionFailure(getFileName(__FILE__), __LINE__, "ASSERTION FAILED", #EXP); \
        }                                                                                      \
        ++successCount;                                                                        \
    } while(0)

#define ASSERT_THROW(TYPE, EXP)                                                                      \
    do {                                                                                             \
        try {                                                                                        \
            (void)(EXP);                                                                             \
            throw AssertionFailure(getFileName(__FILE__), __LINE__, "No exception caught in", #EXP); \
        } catch (TYPE const&) { }                                                                    \
          catch (AssertionFailure const& e) { throw e; }                                             \
          catch (...) {                                                                              \
            throw AssertionFailure(                                                                  \
                getFileName(__FILE__),                                                               \
                __LINE__,                                                                            \
                string{"Catch a exception but not meet the required type: "} + #TYPE,                \
                #EXP);                                                                               \
        }                                                                                            \
        ++successCount;                                                                              \
    } while(0)

#undef DEPEND_ON_COMPILER_SUPPORT
#undef PRIMITIVE_CAT
#undef CAT
}
