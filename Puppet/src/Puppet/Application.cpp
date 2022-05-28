#include "PPpch.h"
#include "Application.h"
#include <glad/glad.h>
#include "Platform/OpenGL/OpenGLBuffer.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"
#include "Renderer/Renderer.h"
namespace Puppet {

	Application* Application::s_Instance = nullptr;
	Application::Application()
	{
		PP_CORE_ASSERT(!s_Instance, "Application Instance is exits");
		s_Instance = this;
		m_Window = Scope<Window>(Window::Create());
		m_Window->SetEventCallback(PP_BIND_EVENT_FN(Application::OnEvent));
		m_Window->SetVSync(false);
		
		Renderer::Init();

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);

	}
	Application::~Application()
	{

	}
	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}
	void Application::PushOverlay(Layer* layer)
	{
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
		Timer timer;
		//渲染层从前往后
		while (m_Running)
		{
			TimeStep timeStep = timer.Reset();
			if (!m_Minimized)
			{
				for (Layer* layer : m_LayerStack)
					layer->OnUpdate(timeStep);
			}
			m_ImGuiLayer->Begin();
			for (Layer* layer : m_LayerStack)
			{
				layer->OnUIRender();
			}
			m_ImGuiLayer->End();
			m_Window->OnUpdate();
		}
	}

}

