// WindowsWindow.h
#pragma once

#include "Puppet/Core/Window.h"
#include "Puppet/Platform/OpenGL/OpenGLContext.h"
struct GLFWwindow;
namespace Puppet {

	// 游戏的窗口
	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();

		void OnUpdate() override;
		unsigned int GetWidth() const override { return m_Data.Width; }
		unsigned int GetHeight() const override { return m_Data.Height; }

		void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
		// 垂直同步设置，防止出现显示撕裂
		void SetVSync(bool enable) override;
		bool IsVSync() const override;
		virtual void* GetNativeWindow()const override { return (void*)m_Window; }
	private:
		virtual void Init(const WindowProps& props);
		virtual void Shutdown();
	private:
		GraphicsContext* m_Context;

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
