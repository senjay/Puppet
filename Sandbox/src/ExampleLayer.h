#pragma once
#include <Puppet.h>
using namespace Puppet;
class ExampleLayer : public Puppet::Layer
{
public:

	ExampleLayer() : Layer("Example")
	{
		m_CameraController = CreateRef<OrthographicCameraController>(1280.0f / 720, true);
		m_ShaderLibrary = CreateScope<ShaderLibrary>();
		uint32_t indices[3] = { 0,1,2 };
		float vertices[3 * 7] = {
			-0.5, -0.5, 0,	1, 0, 0, 1,
			 0.5, -0.5, 0,	0, 1, 0, 1,
			   0,  0.5, 0,	0, 0, 1, 1,
		};

		m_VertexBuffer = VertexBuffer::Create(vertices, sizeof(vertices));
		{
			BufferLayout layout = {
			{ShaderDataType::Float3,"a_Position"},
			{ShaderDataType::Float4,"a_Color"},
			};
			m_VertexBuffer->SetLayout(layout);
		}

		m_IndexBuffer = IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));

		m_VertexArray = VertexArray::Create();
		m_VertexArray->AddVertexBuffer(m_VertexBuffer);
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);

		std::string vertexSrc = R"(
			#version 330 core
			layout(location=0) in vec3 a_Position;
			layout(location=1) in vec4 a_Color;
			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;
			out vec4 v_Color;
			void main()
			{
				gl_Position=u_ViewProjection*u_Transform*vec4(a_Position,1.0);
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

		//------------second VBO Test------------//
		float vertices2[4 * 5] = {
			-0.5, -0.5, 0,	0,0,
			 0.5, -0.5, 0,	1,0,
			 0.5,  0.5, 0,	1,1,
			-0.5,  0.5, 0,	0,1
		};
		uint32_t indices2[6] = { 0,1,2,2,3,0 };
		Ref<VertexBuffer>QuadVBuffer;
		QuadVBuffer = VertexBuffer::Create(vertices2, sizeof(vertices2));
		{
			BufferLayout layout = {
			{ShaderDataType::Float3,"a_Position"},
			{ShaderDataType::Float2,"a_TexCoord"}
			};
			QuadVBuffer->SetLayout(layout);
		}
		Ref<IndexBuffer>QuadIndexBuff;
		QuadIndexBuff = IndexBuffer::Create(indices2, sizeof(indices2) / sizeof(uint32_t));
		m_QuadVertexArray = VertexArray::Create();
		m_QuadVertexArray->AddVertexBuffer(QuadVBuffer);
		m_QuadVertexArray->SetIndexBuffer(QuadIndexBuff);
		std::string vertexSrc2 = R"(
			#version 330 core
			layout(location=0) in vec3 a_Position;
			out vec4 v_Position;
			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;
			void main()
			{
				gl_Position=u_ViewProjection*u_Transform*vec4(a_Position,1.0);
				v_Position=vec4(a_Position,1.0);
			}
		)";
		std::string fargmentSrc2 = R"(
			#version 330 core
			layout(location=0) out vec4 color;
			in vec4 v_Position;
			uniform vec4 u_Color;
			void main()
			{
				color=u_Color;
			}
		)";
		m_Shader2 = Shader::Create("QuadShader", vertexSrc2, fargmentSrc2);

		m_ShaderLibrary->Add(m_Shader2->GetName(), m_Shader2);
		m_ShaderLibrary->Load("TriangleShader", vertexSrc, fargmentSrc);
		auto TextureShader = m_ShaderLibrary->Load("./assets/shaders/Texture.glsl");
		m_LogoTexture = Texture2D::Create("./assets/textures/Logo.png");
		m_Texture = Texture2D::Create("./assets/textures/Checkerboard.png");
		TextureShader->Bind();
		TextureShader->SetInt("u_Texture", 0);
	}

	void OnUpdate(TimeStep ts) override
	{
		PP_TRACE("Delta time: {0}s ,{1}ms", ts.GetSeconds(), ts.GetMillseconds());
		float tsSec = ts.GetSeconds();
		m_FPS = static_cast<int>(1.0 / tsSec);

		//camera Update
		m_CameraController->OnUpdate(ts);


		//Render
		RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		RenderCommand::Clear();

		Renderer::BeginScene(m_CameraController->GetCamera());

		static glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

		m_ShaderLibrary->Get("QuadShader")->Bind();
		m_ShaderLibrary->Get("QuadShader")->SetFloat4("u_Color", m_QuadColor);
		for (int i = 0; i < 20; ++i)
		{
			for (int j = 0; j < 20; ++j)
			{
				glm::vec3 pos = { i * 0.11f,j * 0.11f,0.0f };
				glm::mat4 Modeltransform = glm::translate(glm::mat4(1.0f), pos);
				Modeltransform *= scale;//scale
				Renderer::Submit(m_ShaderLibrary->Get("QuadShader"), m_QuadVertexArray, Modeltransform);
			}
		}

		m_Texture->Bind();
		Renderer::Submit(m_ShaderLibrary->Get("Texture"), m_QuadVertexArray, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));
		m_LogoTexture->Bind();
		Renderer::Submit(m_ShaderLibrary->Get("Texture"), m_QuadVertexArray,
			glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));
		Renderer::Submit(m_ShaderLibrary->Get("TriangleShader"), m_VertexArray, glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f)));
		Renderer::EndScene();
	}

	void OnEvent(Puppet::Event& event) override
	{
		//单个改变w或者h,会触发调整camera的m_AspectRatio和glViewport
		//成比例改变w和h,只会触发glViewport
		m_CameraController->OnEvent(event);
	}
	void OnUIRender()override
	{
		ImGui::Begin("Example Layer");
		ImGui::Text("Puppet in Example Layer\n");
		ImGui::Text("FPS: %d\n", m_FPS);
		ImGui::ColorEdit4("Quad Color", glm::value_ptr(m_QuadColor));
		ImGui::End();
	}
	bool OnKeyPressedEvent(KeyPressedEvent& event)
	{
		return false;
		switch (event.GetKeyCode())
		{
		case Puppet::Key::Up:

			break;
		case Puppet::Key::Down:

			break;
		case Puppet::Key::Left:

			break;
		case Puppet::Key::Right:

			break;
		}
		return false;
	}
private:
	Scope<ShaderLibrary>m_ShaderLibrary;
	Ref<VertexBuffer>m_VertexBuffer;
	Ref<IndexBuffer>m_IndexBuffer;
	Ref<VertexArray>m_VertexArray;

	Ref<Shader>m_Shader2;
	Ref<VertexArray>m_QuadVertexArray;
	Ref<OrthographicCameraController>m_CameraController;
	Ref<Texture2D>m_Texture, m_LogoTexture;
	glm::vec4 m_QuadColor = { 0.0f,0.0f,0.0f,1.0f };

	int m_FPS = 0;
};