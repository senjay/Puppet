#include "PPpch.h"
#include "OpenGLContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <GL/GL.h>

namespace Puppet {

	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
		: m_WindowHandle(windowHandle)
	{
		PP_CORE_ASSERT(windowHandle, "Window handle is null!")
	}

	void OpenGLContext::Init()
	{
		// 将当前的游戏窗口和OpenGL绑定// OpenGL C API
		glfwMakeContextCurrent(m_WindowHandle);
		// 初始化Glad，方便之后调用简化渲染
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		PP_CORE_ASSERT(status, "Failed to initialize Glad!");

		PP_CORE_INFO("OpenGL Info:");
		PP_CORE_INFO("  Vendor: {0}", glGetString(GL_VENDOR));    // vendor - 发行商
		PP_CORE_INFO("  Renderer: {0}", glGetString(GL_RENDERER));// Renderer - 使用的什么来渲染
		PP_CORE_INFO("  Version: {0}", glGetString(GL_VERSION));  // Version - 显卡版本号
	}

	void OpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers(m_WindowHandle);
	}

}
