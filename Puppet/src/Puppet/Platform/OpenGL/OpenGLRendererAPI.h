#pragma once
#include "Puppet/Renderer/RendererAPI.h"

namespace Puppet {

	class OpenGLRendererAPI : public RendererAPI
	{
	public:
		virtual void Init()override;
		virtual void Shutdown()override;
		virtual void SetClearColor(const glm::vec4& color) override;
		virtual void Clear(const glm::vec4& color) override;
		virtual void DrawIndexed(uint32_t count, PrimitiveType type, bool depthTest = true)override;
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)override;
	};


}

