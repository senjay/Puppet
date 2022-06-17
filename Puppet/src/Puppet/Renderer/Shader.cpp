#include "PPpch.h"
#include "Shader.h"
#include "Puppet/Renderer/Renderer.h"
#include "Puppet/Platform/OpenGL/OpenGLShader.h"
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
namespace Puppet {
	Ref<Shader> Shader::Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    PP_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateRef<OpenGLShader>(name,vertexSrc, fragmentSrc);
		}

		PP_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<Shader> Shader::Create(const std::string& glslpath)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    PP_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateRef<OpenGLShader>(glslpath);
		}

		PP_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}