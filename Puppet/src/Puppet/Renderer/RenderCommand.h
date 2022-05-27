#pragma once
#include "RendererAPI.h"
namespace Puppet {

	class RenderCommand
	{
	public:
		static void SetClearColor(const glm::vec4& color)
		{
			s_RendererAPI->SetClearColor(color);
		}
		static void Clear()
		{
			s_RendererAPI->Clear();
		}
		static void DrawIndexed(const Ref<VertexArray>& vertexArray)
		{
			s_RendererAPI->DrawIndexed(vertexArray);
		}
		static void Init()
		{
			s_RendererAPI->Init();
		}
	private:
		static RendererAPI* s_RendererAPI;
	};

}


