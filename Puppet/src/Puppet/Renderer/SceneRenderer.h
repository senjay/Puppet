#pragma once
#include "Puppet/Scene/Scene.h"
#include "Puppet/Renderer/RenderPass.h"
#include "Puppet/Scene/Components.h"
namespace Puppet {
	struct SceneRendererCamera
	{
		SceneRendererCamera() = default;
		SceneRendererCamera(const Camera& camera, const glm::mat4& viewMatrix)
			:Camera(camera), ViewMatrix(viewMatrix) {};
		Camera Camera;
		glm::mat4 ViewMatrix;
	};
	class SceneRenderer
	{
	public:
		static void Init();
		static void SetViewportSize(uint32_t width, uint32_t height);

		static void BeginScene(const Scene* scene, const SceneRendererCamera& camera);
		static void EndScene();
		//TODO :now use quad
		static void SubmitMesh(const glm::mat4& transform, SpriteRendererComponent& src, int entityid=-1);
		static void SubmitMesh(const glm::mat4& transform, MeshComponent& src, int entityid = -1);
		static Ref<RenderPass> GetFinalRenderPass();

		static RendererID SceneRenderer::GetFinalColorBufferRendererID();
		static void OnImGuiRender();
	private:
		static void FlushDrawList();
		static void GeometryPass();
	};
}

