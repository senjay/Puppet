#pragma once
#include "Base.h"
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
			Application(const std::string& name="Puppet App");
			virtual ~Application();
			static Application& Get() { return *s_Instance; }
			Window& GetWindow() { return *m_Window; }
			void Run();
			void Close();
			void OnEvent(Event& e);
			void PushLayer(Layer* layer);
			void PushOverlay(Layer* layer);
			void RenderImGui();
			void FlushFPS(float deltaTime);
			ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; };
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

	private:
		float mAvgDuration{ 0.f };
		int   mFrameCount{ 0 };
		int   m_FPS{ 0 };
		static const float s_FPSAlpha;
	};
	// To be defined in Client
	Application* CreateApplication();
}


