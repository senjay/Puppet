#include "PPpch.h"
#ifdef PP_PLATFORM_WINDOWS
#include "Puppet/Platform/Windows/WindowsWindow.h"
#endif // PP_PALTFORM_WINDOWS

namespace Puppet {
	Scope<Window> Window::Create(const WindowProps& props)
	{
		#ifdef PP_PLATFORM_WINDOWS
				return CreateScope<WindowsWindow>(props);
		
		#elif
			PP_CORE_ASSERT(flase, "Unknow Platform!");
			return nullptr;
		#endif // PP_PALTFORM_WINDOWS
	}
}