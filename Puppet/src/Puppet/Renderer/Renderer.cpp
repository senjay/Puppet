#include "PPpch.h"
#include "Renderer.h"
#include "Renderer2D.h"
#include "Pipeline.h"
#include "RendererAPI.h"
#include "SceneRenderer.h"
namespace Puppet {

	
	struct RendererData
	{
		Ref<RenderPass> m_ActiveRenderPass;
		RenderCommandQueue m_CommandQueue;
		Ref<ShaderLibrary> m_ShaderLibrary;

		Ref<VertexBuffer> m_FullscreenQuadVertexBuffer;
		Ref<IndexBuffer> m_FullscreenQuadIndexBuffer;
		Ref<Pipeline> m_FullscreenQuadPipeline;
	};
	static RendererData s_Data;
	void Renderer::Init()
	{
		PP_PROFILE_FUNCTION();

		s_Data.m_ShaderLibrary = CreateRef<ShaderLibrary>();
		Renderer::GetShaderLibrary()->Load("./assets/shaders/Texture2D.glsl");
		Renderer::GetShaderLibrary()->Load("./assets/shaders/Texture3D.glsl");
		
		Renderer::Submit([]() { RenderCommand::Init(); });


		SceneRenderer::Init();

		// Create fullscreen quad
		float x = -1;
		float y = -1;
		float width = 2, height = 2;
		struct QuadVertex
		{
			glm::vec3 Position;
			glm::vec2 TexCoord;
		};

		QuadVertex* data = new QuadVertex[4];

		data[0].Position = glm::vec3(x, y, 0.1f);
		data[0].TexCoord = glm::vec2(0, 0);

		data[1].Position = glm::vec3(x + width, y, 0.1f);
		data[1].TexCoord = glm::vec2(1, 0);

		data[2].Position = glm::vec3(x + width, y + height, 0.1f);
		data[2].TexCoord = glm::vec2(1, 1);

		data[3].Position = glm::vec3(x, y + height, 0.1f);
		data[3].TexCoord = glm::vec2(0, 1);
		PipelineSpecification pipelineSpecification;
		pipelineSpecification.Layout = {
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float2, "a_TexCoord" }
		};
		s_Data.m_FullscreenQuadPipeline = Pipeline::Create(pipelineSpecification);
		s_Data.m_FullscreenQuadVertexBuffer = VertexBuffer::Create(data, 4 * sizeof(QuadVertex));
		uint32_t indices[6] = { 0, 1, 2, 2, 3, 0, };
		s_Data.m_FullscreenQuadIndexBuffer = IndexBuffer::Create(indices, 6);

		Renderer2D::Init();
		
	}

	void Renderer::Clear(const glm::vec4& color)
	{
		Renderer::Submit([=]() {
			RenderCommand::Clear(color);
		});
	}

	void Renderer::SetClearColor(const glm::vec4& color)
	{
		Renderer::Submit([=]() {
			RenderCommand::SetClearColor(color);
		});
	}


	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		Renderer::Submit([=]() {
			RenderCommand::SetViewport(0, 0, width, height);
		});
	}
	void Renderer::Shutdown()
	{
		Renderer2D::Shutdown();
	}

	void Renderer::WaitAndRender()
	{
		s_Data.m_CommandQueue.Execute();
	}

	void Renderer::BeginRenderPass(Ref<RenderPass> renderPass, bool clear)
	{
		PP_CORE_ASSERT(renderPass, "Render pass cannot be null!");

		// TODO: Convert all of this into a render command buffer
		s_Data.m_ActiveRenderPass = renderPass;

		renderPass->GetSpecification().TargetFramebuffer->Bind();
		//clear color ˇ˘framebuffer
		if (clear)
		{
			Renderer::Clear();
			renderPass->GetSpecification().TargetFramebuffer->ClearAttachment(1, -1);
		}
	}

	void Renderer::EndRenderPass()
	{
		PP_CORE_ASSERT(s_Data.m_ActiveRenderPass, "No active render pass! Have you called Renderer::EndRenderPass twice?");
		s_Data.m_ActiveRenderPass->GetSpecification().TargetFramebuffer->Unbind();
		s_Data.m_ActiveRenderPass = nullptr;
	}

	void Renderer::SubmitQuad(Ref<Texture2D> material, const glm::mat4& transform)
	{
		bool depthTest = true;
		bool cullFace = true;
		if (material)
		{
			material->Bind();
			//depthTest = material->GetFlag(MaterialFlag::DepthTest);
			//cullFace = !material->GetFlag(MaterialFlag::TwoSided);

			//auto shader = material->GetShader();
			//shader->SetMat4("u_Transform", transform);
		}

		/*if (cullFace)
			Renderer::Submit([]() { glEnable(GL_CULL_FACE); });
		else
			Renderer::Submit([]() { glDisable(GL_CULL_FACE); });*/

		s_Data.m_FullscreenQuadVertexBuffer->Bind();
		s_Data.m_FullscreenQuadPipeline->Bind();
		s_Data.m_FullscreenQuadIndexBuffer->Bind();
		Renderer::DrawIndexed(6, PrimitiveType::Triangles, depthTest);
	}

	void Renderer::DrawIndexed(uint32_t count, PrimitiveType type, bool depthTest)
	{
		Renderer::Submit([=]() {
			RenderCommand::DrawIndexed(count, type, depthTest);
		});
	}

	Ref<ShaderLibrary> Renderer::GetShaderLibrary()
	{
		return s_Data.m_ShaderLibrary;
	}

	RenderCommandQueue& Renderer::GetRenderCommandQueue()
	{
		return s_Data.m_CommandQueue;
	}
}
