#pragma once
#include <memory>
#include "Puppet/Utils/Ref.h"
namespace Puppet {

	template<typename T>
	using Scope = std::unique_ptr<T>;

	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}


	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return Ref<T>::Create(std::forward<Args>(args)...);
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


#define PP_EXPAND_MACRO(x) x
#define PP_STRINGIFY_MACRO(x) #x

#define BIT(x) 1<<x

#define PP_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

#include "Puppet/Core/Log.h"
#include "Puppet/Core/Assert.h"