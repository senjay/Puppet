#include <Puppet.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale
#include <glm/ext/matrix_clip_space.hpp> // glm::perspective
#include <glm/ext/scalar_constants.hpp> // glm::pi
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
glm::mat4 camera(float Translate, glm::vec2 const& Rotate)
{
	glm::mat4 Projection = glm::perspective(glm::pi<float>() * 0.25f, 4.0f / 3.0f, 0.1f, 100.f);
	glm::mat4 View = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -Translate));
	View = glm::rotate(View, Rotate.y, glm::vec3(-1.0f, 0.0f, 0.0f));
	View = glm::rotate(View, Rotate.x, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 Model = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
	return Projection * View * Model;
}
using namespace Puppet;
class ExampleLayer : public Puppet::Layer
{
public:
	
	ExampleLayer() : Layer("Example")
	{
		m_Camera = std::make_shared<OrthographicCamera>(-1.6, 1.6, -0.9, 0.9);

		uint32_t indices[3] = { 0,1,2 };
		float vertices[3 * 7] = {
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
		m_Shader=Shader::Create(vertexSrc, fargmentSrc);

		//------------second VBO Test------------//
		float vertices2[4 * 5] = {
			-0.5, -0.5, 0,	0,0,
			 0.5, -0.5, 0,	1,0,
			 0.5,  0.5, 0,	1,1,
			-0.5,  0.5, 0,	0,1
		};
		uint32_t indices2[6] = { 0,1,2,2,3,0 };
		Ref<VertexBuffer>QuadVBuffer;
		QuadVBuffer.reset(VertexBuffer::Create(vertices2, sizeof(vertices2)));
		{
			BufferLayout layout = {
			{ShaderDataType::Float3,"a_Position"},
			{ShaderDataType::Float2,"a_TexCoord"}
			};
			QuadVBuffer->SetLayout(layout);
		}
		Ref<IndexBuffer>QuadIndexBuff;
		QuadIndexBuff.reset(IndexBuffer::Create(indices2, sizeof(indices2) / sizeof(uint32_t)));
		m_QuadVertexArray.reset(VertexArray::Create());
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
		m_Shader2=Shader::Create(vertexSrc2, fargmentSrc2);

		std::string TexturevertexSrc = R"(
			#version 330 core
			layout(location=0) in vec3 a_Position;
			layout(location=1) in vec2 a_TexCoord;
	
			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;
			out vec2 v_TexCoord;
			void main()
			{
				gl_Position=u_ViewProjection*u_Transform*vec4(a_Position,1.0);
				v_TexCoord=a_TexCoord;
			}
		)";
		std::string TexturefargmentSrc = R"(
			#version 330 core
			layout(location=0) out vec4 color;
			in vec2 v_TexCoord;
			uniform sampler2D u_Texture;
			void main()
			{
				color=texture(u_Texture,vec2(v_TexCoord.x,1-v_TexCoord.y));
			}
		)";
		m_TexShader = Shader::Create(TexturevertexSrc, TexturefargmentSrc);
		m_LogoTexture = Texture2D::Create("./assets/textures/Logo.png");
		m_Texture = Texture2D::Create("./assets/textures/Checkerboard.png");
		m_TexShader->Bind();
		m_TexShader->SetInt("u_Texture", 0);
	}

	void OnUpdate(TimeStep ts) override
	{
		PP_TRACE("Delta time: {0}s ,{1}ms", ts.GetSeconds(), ts.GetMillseconds());
		float tsSec = ts.GetSeconds();
		m_FPS =static_cast<int>(1.0 / tsSec);
		if (Puppet::InputSystem::getInstance().IsKeyPressed(Puppet::Key::W))
			m_CameraPosition.y += m_CameraMoveSpeed * tsSec;
		if (Puppet::InputSystem::getInstance().IsKeyPressed(Puppet::Key::S))
			m_CameraPosition.y -= m_CameraMoveSpeed * tsSec;
		if (Puppet::InputSystem::getInstance().IsKeyPressed(Puppet::Key::A))
			m_CameraPosition.x -= m_CameraMoveSpeed * tsSec;
		if (Puppet::InputSystem::getInstance().IsKeyPressed(Puppet::Key::D))
			m_CameraPosition.x += m_CameraMoveSpeed * tsSec;
		if (Puppet::InputSystem::getInstance().IsKeyPressed(Puppet::Key::E))
			m_CameraRotation += m_CameraRotationSpeed * tsSec;
		if (Puppet::InputSystem::getInstance().IsKeyPressed(Puppet::Key::Q))
			m_CameraRotation -= m_CameraRotationSpeed * tsSec;
		RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		RenderCommand::Clear();
		m_Camera->SetRotation(m_CameraRotation);
		m_Camera->SetPosition(m_CameraPosition);

		Renderer::BeginScene(m_Camera);
		
		static glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
		m_Shader2->Bind();
		m_Shader2->SetFloat4("u_Color", m_QuadColor);
		for (int i = 0; i < 20; ++i)
		{
			for (int j = 0; j < 20; ++j)
			{
				glm::vec3 pos = { i * 0.11f,j * 0.11f,0.0f };
				glm::mat4 Modeltransform = glm::translate(glm::mat4(1.0f), pos);
				Modeltransform *= scale;//scale
				Renderer::Submit(m_Shader2, m_QuadVertexArray, Modeltransform);
			}
		}
		
		m_Texture->Bind();
		Renderer::Submit(m_TexShader, m_QuadVertexArray, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));
		m_LogoTexture->Bind();
		Renderer::Submit(m_TexShader, m_QuadVertexArray, 
			glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));
		//Renderer::Submit(m_Shader, m_VertexArray);
		Renderer::EndSence();
	}

	void OnEvent(Puppet::Event& event) override
	{
		
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<KeyPressedEvent>(PP_BIND_EVENT_FN(ExampleLayer::OnKeyPressedEvent));
		if (event.GetEventType() == Puppet::EventType::KeyPressed)
		{
			Puppet::KeyPressedEvent& e = (Puppet::KeyPressedEvent&)event;
			PP_TRACE("{0}", (char)e.GetKeyCode());
		}
	}
	void OnUIRender()override
	{
		ImGui::Begin("Example Layer");
		ImGui::Text("Puppet in Example Layer\n");
		ImGui::Text("FPS: %d\n", m_FPS);
		ImGui::ColorEdit4("Quad Color", glm::value_ptr(m_QuadColor));
		ImGui::End();
	}
	bool OnKeyPressedEvent(KeyPressedEvent&event)
	{
		return false;
		switch (event.GetKeyCode())
		{
		case Puppet::Key::Up :

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
	Ref<Shader>m_Shader;
	Ref<VertexBuffer>m_VertexBuffer;
	Ref<IndexBuffer>m_IndexBuffer;
	Ref<VertexArray>m_VertexArray;

	Ref<Shader>m_Shader2,m_TexShader;
	Ref<VertexArray>m_QuadVertexArray;
	Ref<OrthographicCamera>m_Camera;
	Ref<Texture2D>m_Texture,m_LogoTexture;
	glm::vec3 m_CameraPosition = { 0.0f,0.0f,0.0f };
	float m_CameraRotation = 0.0f;
	float m_CameraMoveSpeed=2.0f;
	float m_CameraRotationSpeed = 10.0f;
	glm::vec4 m_QuadColor = { 0.0f,0.0f,0.0f,1.0f};

	int m_FPS=0;
};

class Sandbox :public Puppet::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
	}
	~Sandbox(){}

};
Puppet::Application* Puppet::CreateApplication()
{
	return new Sandbox();
}
