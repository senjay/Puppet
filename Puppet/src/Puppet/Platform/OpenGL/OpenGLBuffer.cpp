#include "PPpch.h"
#include "OpenGLBuffer.h"
#include "Puppet/Renderer/Renderer.h"
#include "OpenGLUtils.h"
#include <glad/glad.h>

namespace Puppet {	
	//------------------------------------------------------//
	//-----------------VertexBuffer-------------------------//
	//------------------------------------------------------//
	OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size, VertexBufferUsage usage)
	{
		PP_PROFILE_FUNCTION();

		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, OpenGLUtils::OpenGLVBUsage(usage));
		//Ref<OpenGLVertexBuffer> instance = std::dynamic_pointer_cast<OpenGLVertexBuffer>(shared_from_this());
		//Renderer::Submit([instance]() mutable
		//	{
		//		glCreateBuffers(1, &instance->m_RendererID);
		//		glNamedBufferData(instance->m_RendererID, instance->m_Size, instance->m_LocalData.Data, OpenGLUsage(instance->m_Usage));
		//});
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(void* data, uint32_t size, VertexBufferUsage usage)
	{
		PP_PROFILE_FUNCTION();

		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, data, OpenGLUtils::OpenGLVBUsage(usage));
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		PP_PROFILE_FUNCTION();

		glDeleteBuffers(1, &m_RendererID);
	}

	void OpenGLVertexBuffer::Bind() const
	{
		PP_PROFILE_FUNCTION();

		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	}

	void OpenGLVertexBuffer::Unbind() const
	{
		PP_PROFILE_FUNCTION();

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void OpenGLVertexBuffer::SetData(const void* data, uint32_t size)
	{
		PP_PROFILE_FUNCTION();

		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
	}

	//------------------------------------------------------//
	//-----------------IndexBuffer-------------------------//
	//------------------------------------------------------//
	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t * indices, uint32_t count)
		: m_Count(count)
	{
		PP_PROFILE_FUNCTION();

		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		PP_PROFILE_FUNCTION();

		glDeleteBuffers(1, &m_RendererID);
	}

	void OpenGLIndexBuffer::Bind() const
	{
		PP_PROFILE_FUNCTION();

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
	}

	void OpenGLIndexBuffer::Unbind() const
	{
		PP_PROFILE_FUNCTION();

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

}

