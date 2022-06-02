#include "PPpch.h"
#include "Application.h"
#include <glad/glad.h>
#include "Puppet/Platform/OpenGL/OpenGLBuffer.h"
#include "Puppet/Platform/OpenGL/OpenGLVertexArray.h"
#include "Puppet/Renderer/Renderer.h"
#include "Puppet/Renderer/Renderer2D.h"
namespace Puppet {

	Application* Application::s_Instance = nullptr;
	Application::Application()
	{
		PP_PROFILE_FUNCTION();

		PP_CORE_ASSERT(!s_Instance, "Application Instance is exits");
		s_Instance = this;
		m_Window = Scope<Window>(Window::Create());
		m_Window->SetEventCallback(PP_BIND_EVENT_FN(Application::OnEvent));
		m_Window->SetVSync(false);
		
		//Init Renderer or Renderer2D
		Renderer::Init();

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);

	}
	Application::~Application()
	{
		PP_PROFILE_FUNCTION();

		Renderer::Shutdown();
	}
	void Application::PushLayer(Layer* layer)
	{
		PP_PROFILE_FUNCTION();

		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}
	void Application::PushOverlay(Layer* layer)
	{
		PP_PROFILE_FUNCTION();

		m_LayerStack.PushOverlay(layer);
		layer->OnAttach();
	}
	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		PP_CORE_INFO(e);
		m_Running = false;
		return true;
	}
	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		PP_PROFILE_FUNCTION();

		uint32_t w = e.GetWidth();
		uint32_t h = e.GetHeight();
		if (w== 0 || h== 0)
		{
			m_Minimized = true;
			return false;
		}
		m_Minimized = false;
		Renderer::OnWindowResize(w, h);
		return false;
	}
	void Application::OnEvent(Event& e)
	{
		PP_PROFILE_FUNCTION();

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(PP_BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(PP_BIND_EVENT_FN(Application::OnWindowResize));
		//层:事件的响应是反向的
		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
		{
			if (e.m_Handled)//如果事件已经被处理，则停止,需要先判断，可能是closewindow或者resize
				break;
			(*it)->OnEvent(e);
		}
	}
	void Application::Run()
	{
		PP_PROFILE_FUNCTION();

		Timer timer;
		//渲染层从前往后
		while (m_Running)
		{
			PP_PROFILE_SCOPE("Run Loop");
			TimeStep timeStep = timer.Reset();
			if (!m_Minimized)
			{
				{
					PP_PROFILE_SCOPE("LayerStack OnUpdate");
					for (Layer* layer : m_LayerStack)
						layer->OnUpdate(timeStep);
				}
				m_ImGuiLayer->Begin();
				{
					PP_PROFILE_SCOPE("LayerStack OnUIRender");
					for (Layer* layer : m_LayerStack)
						layer->OnUIRender();
				}
				m_ImGuiLayer->End();
			}
			
			m_Window->OnUpdate();
		}
	}

	void Application::Close()
	{
		m_Running = false;
	}

}

