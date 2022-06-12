#include "PPpch.h"
#include "UniformBuffer.h"
#include "Puppet/Renderer/Renderer.h"
#include "Puppet/Platform/OpenGL/OpenGLUniformBuffer.h"
namespace Puppet {

	Ref<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    PP_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateRef<OpenGLUniformBuffer>(size, binding);
		}

		PP_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}