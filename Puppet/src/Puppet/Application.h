#pragma once
#include "Core.h"
#include "Events/Event.h"
#include "Events/ApplicationEvent.h"
#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"
#include "LayerStack.h"
#include "Window.h"
#include "InputSystem.h"
#include "ImGui/ImGuiLayer.h"
#include "Renderer/Shader.h"
#include "Renderer/Buffer.h"
#include "Renderer/VertexArray.h"
namespace Puppet {
	class PUPPET_API Application
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

			std::shared_ptr<Shader>m_Shader;
			std::shared_ptr<VertexBuffer>m_VertexBuffer;
			std::shared_ptr<IndexBuffer>m_IndexBuffer;
			std::shared_ptr<VertexArray>m_VertexArray;

			std::shared_ptr<Shader>m_Shader2;
			std::shared_ptr<VertexArray>m_QuadVertexArray;
	private:
		static Application* s_Instance;
	};
	// To be defined in Client
	Application* CreateApplication();
}


