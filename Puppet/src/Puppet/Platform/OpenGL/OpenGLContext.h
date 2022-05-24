#pragma once
#include "Puppet/Renderer/GraphicsContext.h"

struct GLFWwindow;
namespace Puppet {

	// OpenGL渲染句柄
	class OpenGLContext : public GraphicsContext
	{
	public:
		// 当前引擎的流程是 窗口 -> 渲染引擎
		// 所以渲染引擎会持有窗口的引用
		OpenGLContext(GLFWwindow* windowHandle);

		virtual void Init() override;
		virtual void SwapBuffers() override;
	private:
		// 当前渲染的窗口
		GLFWwindow* m_WindowHandle;
	};

}
