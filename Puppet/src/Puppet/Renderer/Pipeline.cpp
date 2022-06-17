#include "PPpch.h"
#include "Pipeline.h"
#include "Puppet/Platform/OpenGL/OpenGLPipeline.h"
#include "Puppet/Renderer/Renderer.h"
namespace Puppet {

	Ref<Pipeline> Pipeline::Create(const PipelineSpecification& spec)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:    PP_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateRef<OpenGLPipeline>(spec);
		}
		PP_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}


}
