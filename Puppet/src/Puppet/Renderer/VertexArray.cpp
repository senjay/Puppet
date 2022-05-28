#pragma once
#include "PPpch.h"
#include "VertexArray.h"
#include "Renderer.h"
#include "Puppet/Platform/OpenGL/OpenGLVertexArray.h"

namespace Puppet
{
	Ref<VertexArray> VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:    PP_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:  return CreateRef<OpenGLVertexArray>();
		}

		PP_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}

