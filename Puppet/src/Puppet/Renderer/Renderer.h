#pragma once
#include "RendererAPI.h"
#include "RenderCommand.h"
#include "Puppet/Camera/EditorCamera.h"
#include "Shader.h"
namespace Puppet {
	class Renderer
	{
	public:
		static void Init();
		static void OnWindowResize(uint32_t width, uint32_t height);
		static void BeginScene(const EditorCamera&camera);
		static void EndScene();
		static void Shutdown();
		static void Submit(const Ref<Shader>& shader,const Ref<VertexArray>& vertexArray,const glm::mat4& transform=glm::mat4(1.0f));
		static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
	private:
		struct SceneData {
			glm::mat4 ViewProjectionMatrix;
		};
		static SceneData* m_SceneData;
	};

}