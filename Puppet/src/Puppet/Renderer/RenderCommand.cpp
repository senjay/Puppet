#include "PPpch.h"
#include "RenderCommand.h"
#include "Puppet/Platform/OpenGL/OpenGLRendererAPI.h"
namespace Puppet {
	RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI();

}