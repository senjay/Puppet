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
		// ����ǰ����Ϸ���ں�OpenGL��// OpenGL C API
		glfwMakeContextCurrent(m_WindowHandle);
		// ��ʼ��Glad������֮����ü���Ⱦ
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		PP_CORE_ASSERT(status, "Failed to initialize Glad!");

		PP_CORE_INFO("OpenGL Info:");
		PP_CORE_INFO("  Vendor: {0}", glGetString(GL_VENDOR));    // vendor - ������
		PP_CORE_INFO("  Renderer: {0}", glGetString(GL_RENDERER));// Renderer - ʹ�õ�ʲô����Ⱦ
		PP_CORE_INFO("  Version: {0}", glGetString(GL_VERSION));  // Version - �Կ��汾��
	}

	void OpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers(m_WindowHandle);
	}

}
