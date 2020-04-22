#pragma once

#define SET_PROPERTY(PTR, SET_OP) \
	{\
		using Type = remove_cvref_t<decltype(PTR)>;\
		if constexpr (!IsSpecialization<Type, DiskPtr>::value)\
		{\
				PTR SET_OP;\
		}\
		else\
		{\
			PTR.RegisterSetter([&](auto ptr)\
			{\
				ptr SET_OP; \
			});\
		}\
	}