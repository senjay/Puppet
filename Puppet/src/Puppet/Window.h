// Window.h
#pragma once

#include "PPpch.h"

#include "Puppet/Core.h"
#include "Puppet/Events/Event.h"

namespace Puppet {
	// 定义window属性
	struct WindowProps
	{
		std::string Title;
		unsigned int Width;
		unsigned int Height;

		WindowProps(const std::string& title = "Puppet Engine",
			unsigned int width = 1280,
			unsigned int height = 720)
			:Title(title), Width(width), Height(height)
		{
		}
	};

	// 定义窗口基类
	class PUPPET_API Window
	{
	public:
		// 窗口事件回调函数类型
		using EventCallbackFn = std::function<void(Event&)>;
		virtual ~Window() {}
		// 当前制作的引擎是基于PC窗口来实现，最外层的是窗体，窗体update
		// -> 内容update
		virtual void OnUpdate() = 0;


		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;

		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVSync(bool enable) = 0;
		virtual bool IsVSync() const = 0;
		virtual void* GetNativeWindow()const = 0;

		// 这里定义一个静态的Create方法，参数为WindowProps，全部使用的默认数据
		// 这个方法在WindowsWindow.cpp里面实现
		static Window* Create(const WindowProps& prop = WindowProps());
	};
}

