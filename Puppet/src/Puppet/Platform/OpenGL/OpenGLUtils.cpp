#include "PPpch.h"
#include "OpenGLUtils.h"
namespace Puppet::OpenGLUtils
{
	GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
	{
		switch (type)
		{
		case Puppet::ShaderDataType::Float:    return GL_FLOAT;
		case Puppet::ShaderDataType::Float2:   return GL_FLOAT;
		case Puppet::ShaderDataType::Float3:   return GL_FLOAT;
		case Puppet::ShaderDataType::Float4:   return GL_FLOAT;
		case Puppet::ShaderDataType::Mat3:     return GL_FLOAT;
		case Puppet::ShaderDataType::Mat4:     return GL_FLOAT;
		case Puppet::ShaderDataType::Int:      return GL_INT;
		case Puppet::ShaderDataType::Int2:     return GL_INT;
		case Puppet::ShaderDataType::Int3:     return GL_INT;
		case Puppet::ShaderDataType::Int4:     return GL_INT;
		case Puppet::ShaderDataType::Bool:     return GL_BOOL;
		}
		PP_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}
	GLenum OpenGLVBUsage(VertexBufferUsage usage)
	{
		switch (usage)
		{
		case VertexBufferUsage::Static:    return GL_STATIC_DRAW;
		case VertexBufferUsage::Dynamic:   return GL_DYNAMIC_DRAW;
		}
		PP_CORE_ASSERT(false, "Unknown vertex buffer usage");
		return 0;
	}
	GLenum GetOpenGLTextureFormat(TextureFormat format)
	{
		switch (format)
		{
		case Puppet::TextureFormat::RGB:     return GL_RGB;
		case Puppet::TextureFormat::RGBA:    return GL_RGBA;
		case Puppet::TextureFormat::Float16: return GL_RGBA16F;
		}
		PP_CORE_ASSERT(false, "Unknown texture format!");
		return 0;
	}
}