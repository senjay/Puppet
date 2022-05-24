#pragma once
#include "Puppet/Renderer/GraphicsContext.h"

struct GLFWwindow;
namespace Puppet {

	// OpenGL��Ⱦ���
	class OpenGLContext : public GraphicsContext
	{
	public:
		// ��ǰ����������� ���� -> ��Ⱦ����
		// ������Ⱦ�������д��ڵ�����
		OpenGLContext(GLFWwindow* windowHandle);

		virtual void Init() override;
		virtual void SwapBuffers() override;
	private:
		// ��ǰ��Ⱦ�Ĵ���
		GLFWwindow* m_WindowHandle;
	};

}
