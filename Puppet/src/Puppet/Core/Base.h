#pragma once
#include <memory>
namespace Puppet {

	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>

	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}
	template<typename T>
	using Ref = std::shared_ptr<T>;

	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}

#ifdef  PP_PLATFORM_WINDOWS
	#if PP_DYNAMIC_LINK
		#ifdef  PP_BUILD_DLL
			#define PUPPET_API __declspec(dllexport)
		#else
			#define PUPPET_API __declspec(dllimport)
		#endif 
	#else
		#define PUPPET_API
	#endif
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

#define PP_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

#include "Puppet/Core/Log.h"