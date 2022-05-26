#pragma once
#include "RendererAPI.h"
#include "RenderCommand.h"
#include "OrthographicCamera.h"
#include "Shader.h"
namespace Puppet {
	class Renderer
	{
	public:
		static void BeginScene(std::shared_ptr<OrthographicCamera>&camera);
		static void EndSence();
		static void Submit(const std::shared_ptr<Shader>& shader,const std::shared_ptr<VertexArray>& vertexArray,const glm::mat4& transform=glm::mat4(1.0f));
		static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
	private:
		struct SceneData {
			glm::mat4 ViewProjectionMatrix;
		};
		static SceneData* m_SceneData;
	};

}