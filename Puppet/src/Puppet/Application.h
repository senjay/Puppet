#pragma once
#include "Core.h"
#include "Events/Event.h"
#include "Events/ApplicationEvent.h"
#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"
#include "LayerStack.h"
#include "Window.h"
#include "ImGui/ImGuiLayer.h"
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

			std::unique_ptr<Window> m_Window;
			ImGuiLayer* m_ImGuiLayer;
			bool m_Running = true;
			LayerStack m_LayerStack;

			TimeStep m_TimeStep;
			float m_LastTime=0.0;
	private:
		static Application* s_Instance;
	};
	// To be defined in Client
	Application* CreateApplication();
}


