#pragma once
#include "Core.h"
#include "Puppet/Events/Event.h"
#include "Puppet/Events/ApplicationEvent.h"
#include "Puppet/Events/KeyEvent.h"
#include "Puppet/Events/MouseEvent.h"
#include "LayerStack.h"
#include "Window.h"
#include "Puppet/ImGui/ImGuiLayer.h"
#include "Puppet/Core/TimeStep.h"
#include "Puppet/Core/Timer.h"
namespace Puppet {
	class  Application
	{
	public:
			Application();
			virtual ~Application();
			static Application& Get() { return *s_Instance; }
			Window& GetWindow() { return *m_Window; }
			void Run();
			void OnEvent(Event& e);
			void PushLayer(Layer* layer);
			void PushOverlay(Layer* layer);
	private:
			bool OnWindowClose(WindowCloseEvent& e);
			bool OnWindowResize(WindowResizeEvent& e);
	private:
			Scope<Window> m_Window;
			ImGuiLayer* m_ImGuiLayer;
			bool m_Running = true;
			bool m_Minimized = false;
			LayerStack m_LayerStack;

			TimeStep m_TimeStep;
			float m_LastTime=0.0;
	private:
		static Application* s_Instance;
	};
	// To be defined in Client
	Application* CreateApplication();
}


