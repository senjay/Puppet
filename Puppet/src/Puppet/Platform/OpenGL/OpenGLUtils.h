#pragma once
#include<glad/glad.h>
#include "Puppet/Renderer/VertexBuffer.h"
namespace Puppet::OpenGLUtils {
	GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type);
	GLenum OpenGLVBUsage(VertexBufferUsage usage);
}