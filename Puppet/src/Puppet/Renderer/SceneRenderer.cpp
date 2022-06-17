#include "PPpch.h"
#include "SceneRenderer.h"
#include "UniformBuffer.h"
#include "Renderer.h"
#include "Renderer2D.h"
namespace Puppet {

	struct SceneRendererData
	{
		const Scene* ActiveScene = nullptr;
		struct SceneInfo
		{
			SceneRendererCamera SceneCamera;
		} SceneData;

		Ref<RenderPass> GeoPass;
		Ref<Shader>TextureShader;
		struct DrawCommand
		{
			glm::mat4 Transform;
			glm::vec4 Color;
			int EntityID;
		};
		std::vector<DrawCommand> DrawList;
		struct CameraData
		{
			glm::mat4 ViewProjection;
		};
		CameraData CameraBuffer;
		Ref<UniformBuffer> CameraUniformBuffer;
		Ref<Texture2D> material;
	};
	static SceneRendererData s_Data;
	void SceneRenderer::Init()
	{
		RenderPassSpecification RenderPassSpec;
		FramebufferSpecification fbSpec;
		fbSpec.Attachments = { FramebufferTextureFormat::RGBA8,FramebufferTextureFormat::RED_INTEGER,FramebufferTextureFormat::Depth };
		fbSpec.Samples = 1;
		fbSpec.Width = 1280;
		fbSpec.Height = 720;
		RenderPassSpec.TargetFramebuffer= Framebuffer::Create(fbSpec);
		s_Data.GeoPass = RenderPass::Create(RenderPassSpec);
		s_Data.TextureShader = Renderer::GetShaderLibrary()->Get("Texture3D");
		s_Data.material = Texture2D::Create("./assets/textures/Logo.png");
		//s_Data.TextureShader = Shader::Create("./assets/shaders/Texture2.glsl");
		s_Data.TextureShader->Bind();
		int samplers[32];
		for (int i = 0; i < 32; ++i)
			samplers[i] = i;
		s_Data.TextureShader->SetIntArray("u_Textures", samplers, 32);
		s_Data.CameraUniformBuffer= UniformBuffer::Create(sizeof(SceneRendererData::CameraData), 1);
	}
	void SceneRenderer::SetViewportSize(uint32_t width, uint32_t height)
	{
		s_Data.GeoPass->GetSpecification().TargetFramebuffer->Resize(width, height);
	}
	void SceneRenderer::BeginScene(const Scene* scene, const SceneRendererCamera& camera)
	{
		s_Data.ActiveScene = scene;
		s_Data.SceneData.SceneCamera = camera;
		s_Data.CameraBuffer.ViewProjection = camera.Camera.GetProjection()*camera.ViewMatrix;
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(SceneRendererData::CameraData));
	}
	void SceneRenderer::EndScene()
	{
		s_Data.ActiveScene = nullptr;
		FlushDrawList();
	}
	void SceneRenderer::SubmitMesh(const glm::mat4& transform, const glm::vec4& color,int entityid)
	{
		s_Data.DrawList.push_back({ transform, color,entityid });
	}
	Ref<RenderPass> SceneRenderer::GetFinalRenderPass()
	{
		return s_Data.GeoPass;
	}
	RendererID SceneRenderer::GetFinalColorBufferRendererID()
	{
		return s_Data.GeoPass->GetSpecification().TargetFramebuffer->GetColorAttachmentRendererID(0);
	}
	void SceneRenderer::OnImGuiRender()
	{
	}
	void SceneRenderer::FlushDrawList()
	{
		PP_CORE_ASSERT(!s_Data.ActiveScene);
		GeometryPass();
		s_Data.DrawList.clear();
		//s_Data.SelectedMeshDrawList.clear();
		//s_Data.ShadowPassDrawList.clear();
		//s_Data.SceneData = {};
	}
	void SceneRenderer::GeometryPass()
	{
		Renderer::BeginRenderPass(s_Data.GeoPass);
		//Renderer::WaitAndRender();
		//s_Data.TextureShader->Bind();
		//glm::mat4 t(1.0f);
		//Renderer::SubmitQuad(s_Data.material, t);
		Renderer2D::BeginScene(s_Data.SceneData.SceneCamera.Camera, s_Data.SceneData.SceneCamera.ViewMatrix);
		for (auto& dc : s_Data.DrawList)
		{
			Renderer2D::DrawQuad(dc.Transform, dc.Color, dc.EntityID);
		}
		Renderer2D::EndScene();
		Renderer::EndRenderPass();
	}
}

