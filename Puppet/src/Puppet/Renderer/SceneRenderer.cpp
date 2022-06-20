#include "PPpch.h"
#include "SceneRenderer.h"
#include "UniformBuffer.h"
#include "Renderer.h"
#include "Renderer2D.h"
#include "Puppet/Library/TextureLibrary.h"
#include "Puppet/Library/ShaderLibrary.h"
#include "Puppet/Library/UniformBufferLibrary.h"
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
			SpriteRendererComponent Sprite;
			int EntityID;
		};
		struct DrawCommand2
		{
			glm::mat4 Transform;
			MeshComponent Model;
			int EntityID;
		};
		std::vector<DrawCommand> DrawList;
		std::vector<DrawCommand2> DrawList2;
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
		s_Data.TextureShader = ShaderLibrary::GetInstance().Get("Texture3D");
		s_Data.material = TextureLibrary::GetInstance().Get("DefaultTexture");
		s_Data.TextureShader->Bind();
		int samplers[32];
		for (int i = 0; i < 32; ++i)
			samplers[i] = i;
		s_Data.TextureShader->SetIntArray("u_Textures", samplers, 32);
	}
	void SceneRenderer::SetViewportSize(uint32_t width, uint32_t height)
	{
		s_Data.GeoPass->GetSpecification().TargetFramebuffer->Resize(width, height);
	}
	void SceneRenderer::BeginScene(const Scene* scene, const SceneRendererCamera& camera)
	{
		s_Data.ActiveScene = scene;
		s_Data.SceneData.SceneCamera = camera;
		glm::mat4 ViewProjection = camera.Camera.GetProjection()*camera.ViewMatrix;
		UniformBufferLibrary::GetInstance().Get("CameraUniform")->SetData(&ViewProjection, sizeof(CameraData::ViewProjection));
	}
	void SceneRenderer::EndScene()
	{
		s_Data.ActiveScene = nullptr;
		FlushDrawList();
	}
	void SceneRenderer::SubmitMesh(const glm::mat4& transform, SpriteRendererComponent& src,int entityid)
	{
		s_Data.DrawList.push_back({ transform, src,entityid });
	}
	void SceneRenderer::SubmitMesh(const glm::mat4& transform, MeshComponent& model, int entityid)
	{
		s_Data.DrawList2.push_back({ transform, model,entityid });
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
		s_Data.DrawList2.clear();
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
		Renderer2D::BeginScene();
		for (auto& dc : s_Data.DrawList)
		{
			Renderer2D::DrawSprite(dc.Transform, dc.Sprite, dc.EntityID);
		}
		Renderer2D::EndScene();
		Ref<Shader>Defaultshader = ShaderLibrary::GetInstance().Get("Default");
		for (auto& dc : s_Data.DrawList2)
		{
			glm::vec3 model = glm::vec3(dc.Transform[3]);
			glm::vec3 campos=glm::vec3(glm::inverse(s_Data.SceneData.SceneCamera.ViewMatrix)[3]);
			dc.Model.m_Mesh->Draw(dc.Transform, campos, Defaultshader, dc.EntityID);
		}
		Renderer::EndRenderPass();
	}
}

