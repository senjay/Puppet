#include "PPpch.h"
#include "OpenGLUniformBuffer.h"
#include "Puppet/Renderer/Renderer.h"
#include <glad/glad.h>

namespace Puppet {

	OpenGLUniformBuffer::OpenGLUniformBuffer(uint32_t size, uint32_t binding)
	{
		m_Size = size;
		Renderer::Submit([instance = Ref<OpenGLUniformBuffer>(this), binding]()mutable {
			glCreateBuffers(1, &instance->m_RendererID);
			glNamedBufferData(instance->m_RendererID, instance->m_Size, nullptr, GL_DYNAMIC_DRAW); // TODO: investigate usage hint
			glBindBufferBase(GL_UNIFORM_BUFFER, binding, instance->m_RendererID);
		});
	}

	OpenGLUniformBuffer::~OpenGLUniformBuffer()
	{
		
		Renderer::Submit([instance = Ref<OpenGLUniformBuffer>(this)]() {
			glDeleteBuffers(1, &instance->m_RendererID);
		});
		
	}


	void OpenGLUniformBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
	{
		m_LocalData = Buffer::Copy(data, size);
		m_Size = size;
		Renderer::Submit([instance = Ref<OpenGLUniformBuffer>(this),offset]()mutable {
			glNamedBufferSubData(instance->m_RendererID, offset, instance->m_Size, instance->m_LocalData.Data);
		});
	}

}