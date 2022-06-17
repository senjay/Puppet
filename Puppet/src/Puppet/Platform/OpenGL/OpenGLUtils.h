#pragma once
#include<glad/glad.h>
#include "Puppet/Renderer/VertexBuffer.h"
#include "Puppet/Renderer/Texture.h"
namespace Puppet::OpenGLUtils {
	GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type);
	GLenum OpenGLVBUsage(VertexBufferUsage usage);
	GLenum GetOpenGLTextureFormat(TextureFormat format);
}