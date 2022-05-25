#pragma once
#include "RendererAPI.h"
#include "RenderCommand.h"
namespace Puppet {
	class Renderer
	{
	public:
		static void BeginScene();
		static void EndSence();
		static void Submit(const std::shared_ptr<VertexArray>& vertexArray);
		static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
	private:

	};

}