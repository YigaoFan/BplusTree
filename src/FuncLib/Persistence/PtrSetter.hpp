#pragma once

#define SET_PROPERTY(PTR, CAPTURE_WAY, SET_OP)                           \
	{                                                                    \
		using ::std::decay_t;                                            \
		using Type = decay_t<decltype(PTR)>;                             \
		if constexpr (IsSpecialization<Type, UniqueDiskPtr>::value)      \
		{                                                                \
			PTR.RegisterSetter([CAPTURE_WAY](auto ptr) { ptr SET_OP; }); \
		}                                                                \
		else                                                             \
		{                                                                \
			PTR SET_OP;                                                  \
		}                                                                \
	}