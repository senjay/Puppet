// WindowsWindow.h
#pragma once

#include "Puppet/Window.h"

#include <GLFW/glfw3.h>
namespace Puppet {

	// 游戏的窗口
	class WindowsWindow : public Window
	{
	public:
		// const 方法里面不能修改props的数据
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();

		// override 这个方法从父类重写，不加默认新定义的方法
		void OnUpdate() override;

		// inline 编译期优化，减少栈内存开销，编译时候代码替换
		// const 在方法的后面，这个方法里面的数据不能修改类的其他数据
		inline unsigned int GetWidth() const override { return m_Data.Width; }
		inline unsigned int GetHeight() const override { return m_Data.Height; }

		inline void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
		// 垂直同步设置，防止出现显示撕裂
		void SetVSync(bool enable) override;
		bool IsVSync() const override;
	private:
		virtual void Init(const WindowProps& props);
		virtual void Shutdown();
	private:
		// OpenGL 声明一个窗体
		GLFWwindow* m_Window;

		struct WindowData
		{
			std::string Title;
			unsigned int Width, Height;
			bool VSync;

			EventCallbackFn EventCallback;
		};

		WindowData m_Data;
	};
}
