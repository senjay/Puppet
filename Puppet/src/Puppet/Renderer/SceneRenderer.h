#pragma once
#include "Puppet/Scene/Scene.h"
#include "Puppet/Renderer/RenderPass.h"
#include "Puppet/Scene/Components.h"
namespace Puppet {
	struct SceneRendererOptions
	{
		bool ShowGrid = false;
		bool ShowBoundingBoxes = false;
	};
	struct SceneRendererCamera
	{
		Camera Camera;
		glm::mat4 ViewMatrix;
		float Near;
		float Far;
		float FOV;
	};
	class SceneRenderer
	{
	public:
		static void Init();
		static void SetViewportSize(uint32_t width, uint32_t height);

		static void BeginScene(const Scene* scene, const SceneRendererCamera& camera);
		static void EndScene();
		static std::pair<Ref<TextureCube>, Ref<TextureCube>> CreateEnvironmentMap(const std::string& filepath);

		static void SubmitMesh(Ref<Mesh> mesh, const glm::mat4& transform = glm::mat4(1.0f), Ref<MaterialInstance> overrideMaterial = nullptr);
		static void SubmitSelectedMesh(Ref<Mesh> mesh, const glm::mat4& transform = glm::mat4(1.0f));
		static Ref<RenderPass> GetFinalRenderPass();
		static Ref<Texture2D> GetFinalColorBuffer(){}
		static RendererID GetFinalColorBufferRendererID();
		static SceneRendererOptions& GetOptions();

		static void OnImGuiRender(bool* open);
	private:
		static void FlushDrawList();
		static void ShadowMapPass();
		static void GeometryPass();
		static void CompositePass();
		static void BloomBlurPass();
	};
}

