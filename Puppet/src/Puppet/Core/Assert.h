#pragma once

#ifdef PP_DEBUG
#define PP_ENABLE_ASSERTS

#if defined(PP_PLATFORM_WINDOWS)
#define PP_DEBUGBREAK() __debugbreak()
#elif defined(PP_PLATFORM_LINUX)
#include <signal.h>
#define PP_DEBUGBREAK() raise(SIGTRAP)
#else
#error "Platform doesn't support debugbreak yet!"
#endif
#else
#define PP_DEBUGBREAK()
#endif // PP_DEBUG


#ifdef PP_ENABLE_ASSERTS
// Alteratively we could use the same "default" message for both "WITH_MSG" and "NO_MSG" and
// provide support for custom formatting by concatenating the formatting string instead of having the format inside the default message
#define PP_INTERNAL_ASSERT_IMPL(type, check, msg, ...) { if(!(check)) { PP##type##ERROR(msg, __VA_ARGS__); PP_DEBUGBREAK(); } }
#define PP_INTERNAL_ASSERT_WITH_MSG(type, check, ...) PP_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
#define PP_INTERNAL_ASSERT_NO_MSG(type, check) PP_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", PP_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)

#define PP_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
#define PP_INTERNAL_ASSERT_GET_MACRO(...) PP_EXPAND_MACRO( PP_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, PP_INTERNAL_ASSERT_WITH_MSG, PP_INTERNAL_ASSERT_NO_MSG) )

// Currently accepts at least the condition and one additional parameter (the message) being optional
#define PP_ASSERT(...) PP_EXPAND_MACRO( PP_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__) )
#define PP_CORE_ASSERT(...) PP_EXPAND_MACRO( PP_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__) )
#else
#define PP_ASSERT(...)
#define PP_CORE_ASSERT(...)
#endif