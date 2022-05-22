#pragma once

#ifdef  PP_PLATFORM_WINDOWS
#ifdef  PP_BUILD_DLL
	#define PUPPET_API __declspec(dllexport)
#else
	#define PUPPET_API __declspec(dllimport)
#endif //  PP_BUILD_DLL
#else
	#error Only Windows Now
#endif //  PP_PLATFORM_WINDOWS

#ifdef PP_DEBUG
	#define PP_ENABLE_ASSERTS
#endif // PP_DEBUG


#ifdef PP_ENABLE_ASSERTS
#define PP_ASSERT(x,...) { if(!(x)){PP_ERROR("Assertion Failed: {0}",__VA_ARGS__);__debugbreak();}}
#define PP_CORE_ASSERT(x,...) { if(!(x)){PP_CORE_ERROR("Assertion Failed: {0}",__VA_ARGS__);__debugbreak();}}
#else
#define PP_ASSERT(...)
#define PP_CORE_ASSERT(...)
#endif


#define BIT(x) 1<<x

#define PP_BIND_EVENT_FN(fn) std::bind(&fn,this,std::placeholders::_1)
