#pragma once
#include "Core.h"
#include "Events/Event.h"
#include "Events/ApplicationEvent.h"
#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"
#include "LayerStack.h"
#include "Window.h"

namespace Puppet {
	class HAZEL_API Application
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
			static Application* s_Instance;
			std::unique_ptr<Window> m_Window;
			bool m_Running = true;
			LayerStack m_LayerStack;
	};
	// To be defined in Client
	Application* CreateApplication();
}


