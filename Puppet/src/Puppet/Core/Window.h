// Window.h
#pragma once
#include "Puppet/Core/Base.h"
#include "Puppet/Events/Event.h"

namespace Puppet {
	// 定义window属性
	struct WindowProps
	{
		std::string Title;
		uint32_t Width;
		uint32_t Height;

		WindowProps(const std::string& title = "Puppet Engine",
			uint32_t width = 1600,
			uint32_t height = 900)
			:Title(title), Width(width), Height(height)
		{
		}
	};

	// 定义窗口基类
	class Window
	{
	public:
		// 窗口事件回调函数类型
		using EventCallbackFn = std::function<void(Event&)>;
		virtual ~Window() {}
		// 当前制作的引擎是基于PC窗口来实现，最外层的是窗体，窗体update
		// -> 内容update
		virtual void OnUpdate() = 0;


		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVSync(bool enable) = 0;
		virtual bool IsVSync() const = 0;
		virtual void* GetNativeWindow()const = 0;

		// 这里定义一个静态的Create方法，参数为WindowProps，全部使用的默认数据
		// 这个方法在WindowsWindow.cpp里面实现
		static Scope<Window> Create(const WindowProps& prop = WindowProps());
	};
}

