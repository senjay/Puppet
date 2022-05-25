#include "PPpch.h"
#include "Renderer.h"
namespace Puppet {
	void Renderer::BeginScene()
	{

	}
	void Renderer::EndSence()
	{

	}
	void Renderer::Submit(const std::shared_ptr<VertexArray>& vertexArray)
	{
		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}
}
