#include "PPpch.h"
#include "Renderer2D.h"
#include "VertexArray.h"
#include "Shader.h"
#include "RenderCommand.h"
namespace Puppet {

	struct Renderer2DStorage
	{
		Ref<VertexArray>QuadVertexArray;
		Ref<Shader>FlatShader;
	};

	static Renderer2DStorage* s_Data;

	void Renderer2D::Init()
	{
		s_Data = new Renderer2DStorage();

		s_Data->QuadVertexArray = VertexArray::Create();
		
		float quadVertices[4 * 5] = {
		-0.5, -0.5, 0,
		 0.5, -0.5, 0,
		 0.5,  0.5, 0,
		-0.5,  0.5, 0,
		};
		uint32_t indices[6] = { 0,1,2,2,3,0 };
		Ref<VertexBuffer>QuadVBuffer;
		QuadVBuffer = VertexBuffer::Create(quadVertices, sizeof(quadVertices));
		{
			BufferLayout layout = {
			{ShaderDataType::Float3,"a_Position"},
			};
			QuadVBuffer->SetLayout(layout);
		}
		Ref<IndexBuffer>QuadIndexBuff;
		QuadIndexBuff = IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));
		
		s_Data->QuadVertexArray->AddVertexBuffer(QuadVBuffer);
		s_Data->QuadVertexArray->SetIndexBuffer(QuadIndexBuff);
		s_Data->FlatShader = Shader::Create("./assets/shaders/FlatColor.glsl");
	}
	void Renderer2D::Shutdown()
	{
		delete s_Data;
	}
	void Renderer2D::BeginScene(OrthographicCamera& camera)
	{
		s_Data->FlatShader->Bind();
		s_Data->FlatShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
		s_Data->FlatShader->SetMat4("u_Transform", glm::mat4(1.0));
	}
	void Renderer2D::EndSence()
	{
	}
	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad({ position.x,position.y,0 }, size, color);
	}
	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		s_Data->FlatShader->Bind();
		s_Data->FlatShader->SetFloat4("u_Color", color);
		s_Data->QuadVertexArray->Bind();
		RenderCommand::DrawIndexed(s_Data->QuadVertexArray);
	}
}
