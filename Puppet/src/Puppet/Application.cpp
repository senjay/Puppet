#include "PPpch.h"
#include "Application.h"
#include <glad/glad.h>
#include "Platform/OpenGL/OpenGLBuffer.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"
namespace Puppet {

	Application* Application::s_Instance = nullptr;
	Application::Application()
	{
		PP_CORE_ASSERT(!s_Instance, "Application Instance is exits");
		s_Instance = this;
		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback(PP_BIND_EVENT_FN(Application::OnEvent));

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);


		uint32_t indices[3] = { 0,1,2 };
		float vertices[3*7] = {
			-0.5, -0.5, 0,	1, 0, 0, 1,
			 0.5, -0.5, 0,	0, 1, 0, 1,
			   0,  0.5, 0,	0, 0, 1, 1,
		};

		m_VertexBuffer.reset(VertexBuffer::Create(vertices, sizeof(vertices)));
		{
			BufferLayout layout = {
			{ShaderDataType::Float3,"a_Position"},
			{ShaderDataType::Float4,"a_Color"},
			};
			m_VertexBuffer->SetLayout(layout);
		}

		m_IndexBuffer.reset(IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		
		m_VertexArray.reset(VertexArray::Create());
		m_VertexArray->AddVertexBuffer(m_VertexBuffer);
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);
	
		std::string vertexSrc = R"(
			#version 330 core
			layout(location=0) in vec3 a_Position;
			layout(location=1) in vec4 a_Color;
			out vec4 v_Color;
			void main()
			{
				gl_Position=vec4(a_Position,0.5);
				v_Color=a_Color;
			}
		)";
		std::string fargmentSrc = R"(
			#version 330 core
			layout(location=0) out vec4 color;
			in vec4 v_Color;
			void main()
			{
				color=v_Color;
			}
		)";
		m_Shader = std::make_unique<Shader>(vertexSrc, fargmentSrc);


		//------------second VBO Test------------//
		float vertices2[4 * 3] = {
			-0.5, -0.5, 0,
			 0.5, -0.5, 0,
			 0.5,  0.5, 0,
			-0.5,  0.5, 0,
		};
		uint32_t indices2[6] = { 0,1,2,2,3,0 };
		std::shared_ptr<VertexBuffer>QuadVBuffer;
		QuadVBuffer.reset(VertexBuffer::Create(vertices2, sizeof(vertices2)));
		{
			BufferLayout layout = {
			{ShaderDataType::Float3,"a_Position"}
			};
			QuadVBuffer->SetLayout(layout);
		}
		std::shared_ptr<IndexBuffer>QuadIndexBuff;
		QuadIndexBuff.reset(IndexBuffer::Create(indices2, sizeof(indices2) / sizeof(uint32_t)));
		m_QuadVertexArray.reset(VertexArray::Create());
		m_QuadVertexArray->AddVertexBuffer(QuadVBuffer);
		m_QuadVertexArray->SetIndexBuffer(QuadIndexBuff);
		std::string vertexSrc2 = R"(
			#version 330 core
			layout(location=0) in vec3 a_Position;
			out vec4 v_Position;
			void main()
			{
				gl_Position=vec4(a_Position,1.0);
				v_Position=gl_Position;
			}
		)";
		std::string fargmentSrc2 = R"(
			#version 330 core
			layout(location=0) out vec4 color;
			in vec4 v_Position;
			void main()
			{
				color=vec4((v_Position+0.5).xyz,1);
			}
		)";
		m_Shader2 = std::make_unique<Shader>(vertexSrc2, fargmentSrc2);
	}
	Application::~Application()
	{

	}
	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
	}
	void Application::PushOverlay(Layer* layer)
	{
		m_LayerStack.PushOverlay(layer);
	}
	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		PP_CORE_INFO(e);
		m_Running = false;
		return true;
	}
	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(PP_BIND_EVENT_FN(Application::OnWindowClose));
		//dispatcher.Dispatch<WindowResizeEvent>(PP_BIND_EVENT_FN(Application::OnWindowResize));
		//��:�¼�����Ӧ�Ƿ����
		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
		{
			if (e.m_Handled)//����¼��Ѿ���������ֹͣ,��Ҫ���жϣ�������closewindow����resize
				break;
			(*it)->OnEvent(e);
		}
	}
	void Application::Run()
	{
		
		//��Ⱦ���ǰ����
		
		while (m_Running)
		{


			RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
			RenderCommand::Clear();

			Renderer::BeginScene();

			m_Shader->Bind();
			Renderer::Submit(m_VertexArray);

			m_Shader2->Bind();
			Renderer::Submit(m_QuadVertexArray);

			Renderer::EndSence();


			for (Layer* layer : m_LayerStack)
			{
				layer->OnUpdate();
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

