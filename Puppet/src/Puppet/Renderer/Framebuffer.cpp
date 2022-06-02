#include "PPpch.h"
#include "FrameBuffer.h"
#include "Puppet/Renderer/Renderer.h"
#include "Puppet/Platform/OpenGL/OpenGLFramebuffer.h"
namespace Puppet {

	Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    PP_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateRef<OpenGLFramebuffer>(spec);
		}

		PP_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}