#pragma once

#define SET_PROPERTY(PTR, SET_OP) \
	{\
		using ::FuncLib::DiskPtr;\
		using ::std::decay_t;\
		using Type = decay_t<decltype(PTR)>;\
		if constexpr (IsSpecialization<Type, DiskPtr>::value)\
		{\
			PTR.RegisterSetter([&](auto ptr) { ptr SET_OP; });\
		}\
		else\
		{\
			PTR SET_OP;\
		}\
	}