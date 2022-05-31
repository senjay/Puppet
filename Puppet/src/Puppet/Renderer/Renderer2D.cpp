#include "PPpch.h"
#include "Renderer2D.h"
#include "VertexArray.h"
#include "Shader.h"
#include "RenderCommand.h"
#include <glm/ext/matrix_transform.hpp>
namespace Puppet {

	struct Renderer2DStorage
	{
		Ref<VertexArray>QuadVertexArray;
		Ref<Shader>TextureShader;
		Ref<Texture2D>WhiteTexture;
	};

	static Renderer2DStorage* s_Data;

	void Renderer2D::Init()
	{
		PP_PROFILE_FUNCTION();

		s_Data = new Renderer2DStorage();

		s_Data->QuadVertexArray = VertexArray::Create();
		
		float quadVertices[4 * 5] = {
		-0.5, -0.5, 0, 0,0,
		 0.5, -0.5, 0, 1,0,
		 0.5,  0.5, 0, 1,1,
		-0.5,  0.5, 0, 0,1
		};
		uint32_t indices[6] = { 0,1,2,2,3,0 };
		Ref<VertexBuffer>QuadVBuffer;
		QuadVBuffer = VertexBuffer::Create(quadVertices, sizeof(quadVertices));
		BufferLayout layout = {
		{ShaderDataType::Float3,"a_Position"},
		{ShaderDataType::Float2,"a_TexCoord"},
		};
		QuadVBuffer->SetLayout(layout);
		Ref<IndexBuffer>QuadIndexBuff;
		QuadIndexBuff = IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));
		
		s_Data->QuadVertexArray->AddVertexBuffer(QuadVBuffer);
		s_Data->QuadVertexArray->SetIndexBuffer(QuadIndexBuff);

		s_Data->WhiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		s_Data->WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

		s_Data->TextureShader = Shader::Create("./assets/shaders/Texture.glsl");
		s_Data->TextureShader->Bind();
		s_Data->TextureShader->SetInt("u_Texture", 0);


	}
	void Renderer2D::Shutdown()
	{
		PP_PROFILE_FUNCTION();

		delete s_Data;
	}
	void Renderer2D::BeginScene(OrthographicCamera& camera)
	{
		PP_PROFILE_FUNCTION();

		s_Data->TextureShader->Bind();
		s_Data->TextureShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
	}
	void Renderer2D::EndSence()
	{
		PP_PROFILE_FUNCTION();

	}
	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad({ position.x,position.y,0 }, size, color);
	}
	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		PP_PROFILE_FUNCTION();

		s_Data->WhiteTexture->Bind();
		s_Data->TextureShader->Bind();
		s_Data->TextureShader->SetFloat("m_TilingFactor", 1.0f);
		s_Data->TextureShader->SetFloat4("u_Color", color);
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), {size.x,size.y,1.0f});
		s_Data->TextureShader->SetMat4("u_Transform", transform);
		s_Data->QuadVertexArray->Bind();
		RenderCommand::DrawIndexed(s_Data->QuadVertexArray);
	}
	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, const float tilingFactor,const glm::vec4& tintColor)
	{
		DrawQuad({ position.x,position.y,0 }, size, texture,tilingFactor, tintColor);
	}
	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, const float tilingFactor, const glm::vec4& tintColor)
	{
		PP_PROFILE_FUNCTION();

		texture->Bind();
		s_Data->TextureShader->Bind();
		s_Data->TextureShader->SetFloat("m_TilingFactor", tilingFactor);
		s_Data->TextureShader->SetFloat4("u_Color", tintColor);
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { size.x,size.y,1.0f });
		s_Data->TextureShader->SetMat4("u_Transform", transform);
		s_Data->QuadVertexArray->Bind();
		RenderCommand::DrawIndexed(s_Data->QuadVertexArray);
	}
	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, const float rotation, const glm::vec4& color)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, color);
	}
	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, const float rotation, const glm::vec4& color)
	{
		PP_PROFILE_FUNCTION();

		s_Data->WhiteTexture->Bind();
		s_Data->TextureShader->Bind();
		s_Data->TextureShader->SetFloat("m_TilingFactor", 1.0f);
		s_Data->TextureShader->SetFloat4("u_Color", color);
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			*glm::rotate(glm::mat4(1.0f), glm::radians(rotation), {0,0,1})
			*glm::scale(glm::mat4(1.0f), { size.x,size.y,1.0f });
		s_Data->TextureShader->SetMat4("u_Transform", transform);
		s_Data->QuadVertexArray->Bind();
		RenderCommand::DrawIndexed(s_Data->QuadVertexArray);
	}
	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, const float rotation, const Ref<Texture2D>& texture, const float tilingFactor, const glm::vec4& tintColor)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, texture, tilingFactor, tintColor);
	}
	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, const float rotation, const Ref<Texture2D>& texture, const float tilingFactor, const glm::vec4& tintColor)
	{
		PP_PROFILE_FUNCTION();

		texture->Bind();
		s_Data->TextureShader->Bind();
		s_Data->TextureShader->SetFloat("m_TilingFactor", tilingFactor);
		s_Data->TextureShader->SetFloat4("u_Color", tintColor);
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0,0,1 })
			* glm::scale(glm::mat4(1.0f), { size.x,size.y,1.0f });
		s_Data->TextureShader->SetMat4("u_Transform", transform);
		s_Data->QuadVertexArray->Bind();
		RenderCommand::DrawIndexed(s_Data->QuadVertexArray);

	}
	void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color, int entityID)
	{

	}
	void Renderer2D::DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor, int entityID)
	{
	}
}
