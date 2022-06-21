#include "PPpch.h"
#include "SceneRenderer.h"
#include "UniformBuffer.h"
#include "Renderer.h"
#include "Renderer2D.h"
#include "Puppet/Library/TextureLibrary.h"
#include "Puppet/Library/ShaderLibrary.h"
#include "Puppet/Library/UniformBufferLibrary.h"
#include <glad/glad.h>
namespace Puppet {

	struct SceneRendererData
	{
		const Scene* ActiveScene = nullptr;
		struct SceneInfo
		{
			SceneRendererCamera SceneCamera;
		} SceneData;

		Ref<RenderPass> GeoPass;
		Ref<RenderPass> CompositePass;
		Ref<RenderPass> BloomBlurPass[2];
		Ref<RenderPass> BloomBlendPass;
		Ref<RenderPass> ShadowMapRenderPass[4];
		
		Ref<Texture2D> BRDFLUT;

		Ref<Shader> CompositeShader;
		Ref<Shader> BloomBlurShader;
		Ref<Shader> BloomBlendShader;
		Ref<Shader> ShadowMapShader;


		float ShadowMapSize = 20.0f;
		float LightDistance = 0.1f;
		glm::mat4 LightMatrices[4];
		glm::mat4 LightViewMatrix;
		float CascadeSplitLambda = 0.91f;
		glm::vec4 CascadeSplits;
		float CascadeFarPlaneOffset = 15.0f, CascadeNearPlaneOffset = -15.0f;
		bool ShowCascades = false;
		bool SoftShadows = true;
		float LightSize = 0.5f;
		float MaxShadowDistance = 200.0f;
		float ShadowFade = 25.0f;
		float CascadeTransitionFade = 1.0f;
		bool CascadeFading = true;

		bool EnableBloom = false;
		float BloomThreshold = 1.5f;

		glm::vec2 FocusPoint = { 0.5f, 0.5f };

		RendererID ShadowMapSampler;

		struct DrawCommand
		{
			MeshComponent Model;
			glm::mat4 Transform;
			int EntityID;
		};
		std::vector<DrawCommand> DrawList;

		struct DrawCommand2
		{
			glm::mat4 Transform;
			SpriteRendererComponent Sprite;
			int EntityID;
		};
		struct DrawCommand3
		{
			MeshComponent Model;
			glm::mat4 Transform;
			int EntityID;
		};
		std::vector<DrawCommand2> DrawList2;
		std::vector<DrawCommand3> DrawList3;
		Ref<Texture2D> material;
		Ref<Shader>TextureShader;
	};
	static SceneRendererData s_Data;
	void SceneRenderer::Init()
	{
		//FramebufferSpecification geoFramebufferSpec;
		//geoFramebufferSpec.Attachments = { FramebufferTextureFormat::RGBA16F, FramebufferTextureFormat::RGBA16F, FramebufferTextureFormat::Depth };
		//geoFramebufferSpec.Samples = 8;
		//geoFramebufferSpec.ClearColor = { 0.1f, 0.1f, 0.1f, 1.0f };

		//RenderPassSpecification geoRenderPassSpec;
		//geoRenderPassSpec.TargetFramebuffer = Framebuffer::Create(geoFramebufferSpec);
		//s_Data.GeoPass = RenderPass::Create(geoRenderPassSpec);

		//FramebufferSpecification compFramebufferSpec;
		//compFramebufferSpec.Attachments = { FramebufferTextureFormat::RGBA8 };
		//compFramebufferSpec.ClearColor = { 0.1f, 0.1f, 0.1f, 1.0f };

		//RenderPassSpecification compRenderPassSpec;
		//compRenderPassSpec.TargetFramebuffer = Framebuffer::Create(compFramebufferSpec);
		//s_Data.CompositePass = RenderPass::Create(compRenderPassSpec);

		//FramebufferSpecification bloomBlurFramebufferSpec;
		//bloomBlurFramebufferSpec.Attachments = { FramebufferTextureFormat::RGBA16F };
		//bloomBlurFramebufferSpec.ClearColor = { 0.1f, 0.1f, 0.1f, 1.0f };

		//RenderPassSpecification bloomBlurRenderPassSpec;
		//bloomBlurRenderPassSpec.TargetFramebuffer = Framebuffer::Create(bloomBlurFramebufferSpec);
		//s_Data.BloomBlurPass[0] = RenderPass::Create(bloomBlurRenderPassSpec);
		//bloomBlurRenderPassSpec.TargetFramebuffer = Framebuffer::Create(bloomBlurFramebufferSpec);
		//s_Data.BloomBlurPass[1] = RenderPass::Create(bloomBlurRenderPassSpec);

		//FramebufferSpecification bloomBlendFramebufferSpec;
		//bloomBlendFramebufferSpec.Attachments = { FramebufferTextureFormat::RGBA8 };
		//bloomBlendFramebufferSpec.ClearColor = { 0.1f, 0.1f, 0.1f, 1.0f };

		//RenderPassSpecification bloomBlendRenderPassSpec;
		//bloomBlendRenderPassSpec.TargetFramebuffer = Framebuffer::Create(bloomBlendFramebufferSpec);
		//s_Data.BloomBlendPass = RenderPass::Create(bloomBlendRenderPassSpec);

		//s_Data.CompositeShader = ShaderLibrary::GetInstance().Get("SceneComposite");
		//s_Data.BloomBlurShader = ShaderLibrary::GetInstance().Get("BloomBlur");
		//s_Data.BloomBlendShader = ShaderLibrary::GetInstance().Get("BloomBlend");
		//s_Data.BRDFLUT = Texture2D::Create("assets/textures/BRDF_LUT.tga");


		//FramebufferSpecification shadowMapFramebufferSpec;
		//shadowMapFramebufferSpec.Width = 4096;
		//shadowMapFramebufferSpec.Height = 4096;
		//shadowMapFramebufferSpec.Attachments = { FramebufferTextureFormat::DEPTH32F };
		//shadowMapFramebufferSpec.ClearColor = { 0.0f, 0.0f, 0.0f, 0.0f };
		//shadowMapFramebufferSpec.NoResize = true;

		//// 4 cascades
		//for (int i = 0; i < 4; i++)
		//{
		//	RenderPassSpecification shadowMapRenderPassSpec;
		//	shadowMapRenderPassSpec.TargetFramebuffer = Framebuffer::Create(shadowMapFramebufferSpec);
		//	s_Data.ShadowMapRenderPass[i] = RenderPass::Create(shadowMapRenderPassSpec);
		//}

		//Renderer::Submit([]()
		//	{
		//		glGenSamplers(1, &s_Data.ShadowMapSampler);

		//		// Setup the shadowmap depth sampler
		//		glSamplerParameteri(s_Data.ShadowMapSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//		glSamplerParameteri(s_Data.ShadowMapSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//		glSamplerParameteri(s_Data.ShadowMapSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		//		glSamplerParameteri(s_Data.ShadowMapSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		//	});


		RenderPassSpecification RenderPassSpec;
		FramebufferSpecification fbSpec;
		fbSpec.Attachments = { FramebufferTextureFormat::RGBA8,FramebufferTextureFormat::RED_INTEGER,FramebufferTextureFormat::Depth };
		fbSpec.ClearColor={ 0.1f, 0.1f, 0.1f, 1.0f };
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
		//s_Data.CompositePass->GetSpecification().TargetFramebuffer->Resize(width, height);
	}
	void SceneRenderer::BeginScene(const Scene* scene, const SceneRendererCamera& camera)
	{
		s_Data.ActiveScene = scene;
		s_Data.SceneData.SceneCamera = camera;
		glm::mat4 ViewProjection = camera.Camera.GetProjection()*camera.ViewMatrix;
		UniformBufferLibrary::GetInstance().Get("CameraUniform")->SetData(&ViewProjection, sizeof(CameraData::ViewProjection));

		s_Data.ActiveScene = scene;
		s_Data.SceneData.SceneCamera = camera;
		//s_Data.SceneData.SkyboxMaterial = scene->m_SkyboxMaterial;
		//s_Data.SceneData.SceneEnvironment = scene->m_Environment;
		//s_Data.SceneData.SceneEnvironmentIntensity = scene->m_EnvironmentIntensity;
		//s_Data.SceneData.ActiveLight = scene->m_Light;
		//s_Data.SceneData.SceneLightEnvironment = scene->m_LightEnvironment;
	}
	void SceneRenderer::EndScene()
	{
		s_Data.ActiveScene = nullptr;
		FlushDrawList();
	}
	void SceneRenderer::SubmitMesh(const glm::mat4& transform, SpriteRendererComponent& src,int entityid)
	{
		s_Data.DrawList2.push_back({ transform, src,entityid });
	}
	void SceneRenderer::SubmitMesh(const glm::mat4& transform, MeshComponent& model, int entityid)
	{
		s_Data.DrawList3.push_back({ model,transform, entityid });
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
		s_Data.DrawList2.clear();
		s_Data.DrawList3.clear();
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
		for (auto& dc : s_Data.DrawList2)
		{
			Renderer2D::DrawSprite(dc.Transform, dc.Sprite, dc.EntityID);
		}
		Renderer2D::EndScene();
		Ref<Shader>Defaultshader = ShaderLibrary::GetInstance().Get("Default");
		for (auto& dc : s_Data.DrawList3)
		{
			glm::vec3 model = glm::vec3(dc.Transform[3]);
			glm::vec3 campos=glm::vec3(glm::inverse(s_Data.SceneData.SceneCamera.ViewMatrix)[3]);
			dc.Model.m_Mesh->Draw(dc.Transform, campos, Defaultshader, dc.EntityID);
		}
		Renderer::EndRenderPass();
	}
}

