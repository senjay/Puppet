#include "PPpch.h"
#include "Renderer2D.h"
#include "VertexArray.h"
#include "Shader.h"
#include "RenderCommand.h"
#include "Pipeline.h"
#include "Puppet/Renderer/Renderer.h"
#include "Puppet/Renderer/UniformBuffer.h"
#include "Puppet/Library/TextureLibrary.h"
#include "Puppet/Library/ShaderLibrary.h"
#include <glm/ext/matrix_transform.hpp>
namespace Puppet {

	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		float TexIndex;
		float TilingFactor;

		int EntityID;//for Editor
		//...
	};
	struct Renderer2DData
	{
		static const  uint32_t MaxQuads = 20000;
		static const  uint32_t MaxVertices = MaxQuads * 4;
		static const  uint32_t MaxIndices = MaxQuads * 6;
		static const uint32_t MaxTextureSlots = 32; // TODO: RenderCaps
		Ref<Pipeline> QuadPipeline;
		Ref<VertexBuffer> QuadVertexBuffer;
		Ref<IndexBuffer> QuadIndexBuffer;
		Ref<Shader>TextureShader;
		Ref<Texture2D>WhiteTexture;
		
		uint32_t QuadIndexCount = 0;

		QuadVertex* QuadVertexBufferBase=nullptr;
		QuadVertex* QuadVertexBufferPtr =nullptr;

		std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;

		uint32_t TextureSlotIndex = 1;// 0:white

		glm::vec4 QuadVertexPositions [4];
		
		Renderer2D::Statistics Stats;

		struct CameraData
		{
			glm::mat4 ViewProjection;
		};
		CameraData CameraBuffer;
		Ref<UniformBuffer> CameraUniformBuffer;
		bool DepthTest = true;
	};

	static Renderer2DData s_Data2D;

	void Renderer2D::Init()
	{
		PP_PROFILE_FUNCTION();

		PipelineSpecification pipelineSpecification;
		pipelineSpecification.Layout = {
			{ShaderDataType::Float3,"a_Position"},
			{ShaderDataType::Float4,"a_Color"},
			{ShaderDataType::Float2,"a_TexCoord"},
			{ShaderDataType::Float,"a_TexIndex"},
			{ShaderDataType::Float,"a_TilingFactor"},
			{ShaderDataType::Int,"a_EntityID"},
		};
		s_Data2D.QuadPipeline = Pipeline::Create(pipelineSpecification);

		s_Data2D.QuadVertexBuffer = VertexBuffer::Create(s_Data2D.MaxVertices * sizeof(QuadVertex));
		s_Data2D.QuadVertexBufferBase = new QuadVertex[s_Data2D.MaxVertices];

		uint32_t* quadIndices = new uint32_t[s_Data2D.MaxIndices];
		uint32_t offset = 0;
		for (uint32_t i = 0; i < s_Data2D.MaxIndices; i += 6)
		{
			quadIndices[i+0] = offset + 0;
			quadIndices[i+1] = offset + 1;
			quadIndices[i+2] = offset + 2;

			quadIndices[i+3] = offset + 2;
			quadIndices[i+4] = offset + 3;
			quadIndices[i+5] = offset + 0;

			offset += 4;
		}

		s_Data2D.QuadIndexBuffer =IndexBuffer::Create(quadIndices, s_Data2D.MaxIndices);
		delete[] quadIndices;

		s_Data2D.WhiteTexture = TextureLibrary::GetInstance().Get("WhiteTexture");
		s_Data2D.TextureShader = ShaderLibrary::GetInstance().Get("Texture2D");
		s_Data2D.TextureShader->Bind();
		
		int samplers[s_Data2D.MaxTextureSlots];
		for (int i = 0; i < s_Data2D.MaxTextureSlots; ++i)
			samplers[i] = i;
		s_Data2D.TextureShader->SetIntArray("u_Textures", samplers, s_Data2D.MaxTextureSlots);

		s_Data2D.TextureSlots[0] = s_Data2D.WhiteTexture;

		s_Data2D.QuadVertexPositions[0] = { -0.5f,-0.5f,0.0f,1.0f };
		s_Data2D.QuadVertexPositions[1] = {  0.5f,-0.5f,0.0f,1.0f };
		s_Data2D.QuadVertexPositions[2] = {  0.5f, 0.5f,0.0f,1.0f };
		s_Data2D.QuadVertexPositions[3] = { -0.5f, 0.5f,0.0f,1.0f };

		s_Data2D.CameraUniformBuffer = UniformBuffer::Create(sizeof(Renderer2DData::CameraData), 0);

	}
	void Renderer2D::Shutdown()
	{
		PP_PROFILE_FUNCTION();

		delete[] s_Data2D.QuadVertexBufferBase;
	}
	void Renderer2D::BeginScene(const Camera& camera, const glm::mat4& viewMatrix)
	{
		PP_PROFILE_FUNCTION();

		s_Data2D.CameraBuffer.ViewProjection = camera.GetProjection() * viewMatrix;
		s_Data2D.CameraUniformBuffer->SetData(&s_Data2D.CameraBuffer, sizeof(Renderer2DData::CameraData));
		StartBatch();
	}
	void Renderer2D::BeginScene(const EditorCamera& camera)
	{
		PP_PROFILE_FUNCTION();

		s_Data2D.CameraBuffer.ViewProjection = camera.GetViewProjection();
		s_Data2D.CameraUniformBuffer->SetData(&s_Data2D.CameraBuffer, sizeof(Renderer2DData::CameraData));

		StartBatch();
	}
	void Renderer2D::EndScene()
	{
		PP_PROFILE_FUNCTION();
		
		Flush();

	}
	void Renderer2D::Flush()
	{
		PP_PROFILE_FUNCTION();

		if (s_Data2D.QuadIndexCount == 0)
			return; // Nothing to draw

		uint32_t dataSize = (uint32_t)((uint8_t*)s_Data2D.QuadVertexBufferPtr - (uint8_t*)s_Data2D.QuadVertexBufferBase);
		s_Data2D.QuadVertexBuffer->SetData((const void*)s_Data2D.QuadVertexBufferBase, dataSize);

		s_Data2D.TextureShader->Bind();
		// Bind textures
		for (uint32_t i = 0; i < s_Data2D.TextureSlotIndex; i++)
			s_Data2D.TextureSlots[i]->Bind(i);

		s_Data2D.QuadVertexBuffer->Bind();
		s_Data2D.QuadPipeline->Bind();
		s_Data2D.QuadIndexBuffer->Bind();
		Renderer::DrawIndexed(s_Data2D.QuadIndexCount, PrimitiveType::Triangles, s_Data2D.DepthTest);
		s_Data2D.Stats.DrawCalls++;
	}
	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad({ position.x,position.y,0 }, size, color);
	}
	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		PP_PROFILE_FUNCTION();
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, color);
	}
	
	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, const float tilingFactor,const glm::vec4& tintColor)
	{
		DrawQuad({ position.x,position.y,0 }, size, texture,tilingFactor, tintColor);
	}
	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, const float tilingFactor, const glm::vec4& tintColor)
	{
		PP_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, texture, tilingFactor, tintColor);
	}
	
	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, const float rotation, const glm::vec4& color)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, color);
	}
	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, const float rotation, const glm::vec4& color)
	{
		PP_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, color);
	}
	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, const float rotation, const Ref<Texture2D>& texture, const float tilingFactor, const glm::vec4& tintColor)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, texture, tilingFactor, tintColor);
	}
	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, const float rotation, const Ref<Texture2D>& texture, const float tilingFactor, const glm::vec4& tintColor)
	{
		PP_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, texture, tilingFactor, tintColor);
	}
	
	void Renderer2D::DrawSprite(const glm::mat4& transform, SpriteRendererComponent& src, int entityID)
	{
		if (src.Texture)
			DrawQuad(transform, src.Texture, src.TilingFactor, src.Color, entityID);
		else
			DrawQuad(transform, src.Color, entityID);
	}
	void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color, int entityID)
	{
		PP_PROFILE_FUNCTION();

		constexpr size_t quadVertexCount = 4;
		const float textureIndex = 0.0f; // White Texture
		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
		const float tilingFactor = 1.0f;

		if (s_Data2D.QuadIndexCount >= Renderer2DData::MaxIndices)
			NextBatch();

		for (size_t i = 0; i < quadVertexCount; i++)
		{
			s_Data2D.QuadVertexBufferPtr->Position = transform * s_Data2D.QuadVertexPositions[i];
			s_Data2D.QuadVertexBufferPtr->Color = color;
			s_Data2D.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_Data2D.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Data2D.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data2D.QuadVertexBufferPtr->EntityID = entityID;
			s_Data2D.QuadVertexBufferPtr++;
		}

		s_Data2D.QuadIndexCount += 6;

		s_Data2D.Stats.QuadCount++;
	}
	void Renderer2D::DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor, int entityID)
	{
		PP_PROFILE_FUNCTION();

		constexpr size_t quadVertexCount = 4;
		constexpr glm::vec2 textureCoords[4] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };

		if (s_Data2D.QuadIndexCount >= Renderer2DData::MaxIndices)
			NextBatch();

		float textureIndex = 0.0f;
		for (uint32_t i = 1; i < s_Data2D.TextureSlotIndex; i++)
		{
			if (*s_Data2D.TextureSlots[i] == *texture)
			{
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			if (s_Data2D.TextureSlotIndex >= Renderer2DData::MaxTextureSlots)
				NextBatch();

			textureIndex = (float)s_Data2D.TextureSlotIndex;
			s_Data2D.TextureSlots[s_Data2D.TextureSlotIndex] = texture;
			s_Data2D.TextureSlotIndex++;
		}

		for (size_t i = 0; i < quadVertexCount; i++)
		{
			s_Data2D.QuadVertexBufferPtr->Position = transform * s_Data2D.QuadVertexPositions[i];
			s_Data2D.QuadVertexBufferPtr->Color = tintColor;
			s_Data2D.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_Data2D.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Data2D.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data2D.QuadVertexBufferPtr->EntityID = entityID;
			s_Data2D.QuadVertexBufferPtr++;
		}

		s_Data2D.QuadIndexCount += 6;

		s_Data2D.Stats.QuadCount++;
	}
	

	void Renderer2D::StartBatch()
	{
		s_Data2D.QuadIndexCount = 0;
		s_Data2D.QuadVertexBufferPtr = s_Data2D.QuadVertexBufferBase;
		s_Data2D.TextureSlotIndex = 1;
	}
	void Renderer2D::NextBatch()
	{
		Flush();
		StartBatch();
	}

	Renderer2D::Statistics Renderer2D::GetStats()
	{
		return s_Data2D.Stats;
	}

	void Renderer2D::ResetStats()
	{
		memset(&s_Data2D.Stats, 0, sizeof(Renderer2D::Statistics));
	}
}
