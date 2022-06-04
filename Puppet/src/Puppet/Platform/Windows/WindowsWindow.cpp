#include "PPpch.h"
#include "WindowsWindow.h"
#include "Puppet/Events/ApplicationEvent.h"
#include "Puppet/Events/KeyEvent.h"
#include "Puppet/Events/MouseEvent.h"
#include "WindowsInput.h"
#include "Puppet/Core/InputSystem.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
namespace Puppet {

	static uint8_t  s_GLFWWindowCount = 0;
	static void GLFWErrorCallback(int error_code, const char* description)
	{
		PP_CORE_ERROR("glfw error ({0}): {1}", error_code, description);
	}
	// step 2
	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		PP_PROFILE_FUNCTION();

		Init(props);
	}

	WindowsWindow::~WindowsWindow()
	{
		PP_PROFILE_FUNCTION();

		Shutdown();
	}

	// step 3
	void WindowsWindow::Init(const WindowProps& props)
	{
		PP_PROFILE_FUNCTION();

		Puppet::InputSystem::getInstance().initialize(new WindowsInput());
		
		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;

		PP_CORE_TRACE("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);

		if (s_GLFWWindowCount==0)
		{
			PP_PROFILE_SCOPE("glfwInit");
			int success = glfwInit();
			PP_CORE_ASSERT(success,"Could not initialize GLFW!");
			glfwSetErrorCallback(GLFWErrorCallback);
		}

		{
			PP_PROFILE_SCOPE("glfwCreateWindow");
			m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);
			++s_GLFWWindowCount;
		}
		m_Context = new OpenGLContext(m_Window);
		m_Context->Init();
		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVSync(true);

		//Set Callback
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow *window,int w,int h) {
			WindowData *data=static_cast<WindowData*>(glfwGetWindowUserPointer(window));
			data->Width = w;
			data->Height = h;

			WindowResizeEvent event(w, h);
			data->EventCallback(event);
		});
		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {
			WindowData* data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
			WindowCloseEvent event;
			data->EventCallback(event);
		});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			WindowData* data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
			switch (action)
			{
				case GLFW_PRESS:
				{
					KeyPressedEvent event(key, 0);
					data->EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent event(key);
					data->EventCallback(event);
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent event(key, 1);
					data->EventCallback(event);
					break;
				}
			}
		});
		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
		{
				WindowData* data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
				switch (action)
				{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent event(button);
					data->EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent event(button);
					data->EventCallback(event);
					break;
				}
			}
		});
		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xoffset, double yoffset)
		{
			WindowData* data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
			MouseScrolledEvent event((float)xoffset, (float)yoffset);
			data->EventCallback(event);
		});
		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xpos, double ypos)
		{
			WindowData* data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
			MouseMovedEvent event((float)xpos, (float)ypos);
			data->EventCallback(event);
		});
		glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				KeyTypedEvent event(keycode);
				data.EventCallback(event);
			});
	}

	void WindowsWindow::Shutdown()
	{
		PP_PROFILE_FUNCTION();

		glfwDestroyWindow(m_Window);
		--s_GLFWWindowCount;

		if (s_GLFWWindowCount == 0)
		{
			glfwTerminate();
		}
	}

	// step 4
	void WindowsWindow::OnUpdate()
	{
		PP_PROFILE_FUNCTION();

		glfwPollEvents();
		m_Context->SwapBuffers();
	}

	void WindowsWindow::SetVSync(bool enable)
	{
		PP_PROFILE_FUNCTION();

		if (enable)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		m_Data.VSync = enable;
	}

	bool WindowsWindow::IsVSync() const
	{
		return m_Data.VSync;
	}
}

