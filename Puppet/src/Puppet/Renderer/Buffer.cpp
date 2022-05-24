#include "PPpch.h"
#include "Buffer.h"
#include "Renderer.h"

#include "Puppet/Platform/OpenGL/OpenGLBuffer.h"

namespace Puppet {

	VertexBuffer* VertexBuffer::Create(float* vertices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::None:    PP_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::OpenGL:  return new OpenGLVertexBuffer(vertices, size);
		}

		PP_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	IndexBuffer* IndexBuffer::Create(uint32_t* indices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::None:    PP_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::OpenGL:  return new OpenGLIndexBuffer(indices, size);
		}

		PP_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}
