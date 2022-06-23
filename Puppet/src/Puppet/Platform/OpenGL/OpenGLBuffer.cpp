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
		: m_RendererID(0), m_Size(size), m_Usage(usage)
	{
		PP_PROFILE_FUNCTION();
		
		Renderer::Submit([instance=Ref<OpenGLVertexBuffer>(this)]() mutable
		{
			glCreateBuffers(1, &instance->m_RendererID);
			glNamedBufferData(instance->m_RendererID, instance->m_Size, nullptr, OpenGLUtils::OpenGLVBUsage(instance->m_Usage));
		});
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(void* data, uint32_t size, VertexBufferUsage usage)
		: m_RendererID(0), m_Size(size), m_Usage(usage)
	{
		PP_PROFILE_FUNCTION();
		m_LocalData = Buffer::Copy(data, size);
		Renderer::Submit([instance = Ref<OpenGLVertexBuffer>(this)]() mutable
		{
			glCreateBuffers(1, &instance->m_RendererID);
			glNamedBufferData(instance->m_RendererID, instance->m_Size, instance->m_LocalData.Data, OpenGLUtils::OpenGLVBUsage(instance->m_Usage));
		});
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		PP_PROFILE_FUNCTION();
		Renderer::Submit([instance = Ref<OpenGLVertexBuffer>(this)]()mutable
		{
			glDeleteBuffers(1, &instance->m_RendererID);
		});
	}

	void OpenGLVertexBuffer::Bind() const
	{
		PP_PROFILE_FUNCTION();

		Renderer::Submit([instance = Ref<const OpenGLVertexBuffer>(this)]() {
			glBindBuffer(GL_ARRAY_BUFFER, instance->m_RendererID);
		});
	}


	void OpenGLVertexBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
	{
		PP_PROFILE_FUNCTION();

		m_LocalData = Buffer::Copy(data, size);
		m_Size = size;
		Renderer::Submit([instance = Ref<OpenGLVertexBuffer>(this), offset]() {
			glNamedBufferSubData(instance->m_RendererID, offset, instance->m_Size, instance->m_LocalData.Data);
		});
	}



	//------------------------------------------------------//
	//-----------------IndexBuffer-------------------------//
	//------------------------------------------------------//
	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t count)
		:m_RendererID(0), m_Count(count)
	{
		Renderer::Submit([instance = Ref<OpenGLIndexBuffer>(this)]() mutable {
			glCreateBuffers(1, &instance->m_RendererID);
			glNamedBufferData(instance->m_RendererID, instance->m_Count * sizeof(uint32_t), nullptr, GL_DYNAMIC_DRAW);
		});
	}

	OpenGLIndexBuffer::OpenGLIndexBuffer(const void* indices, uint32_t count)
		: m_RendererID(0), m_Count(count)
	{
		PP_PROFILE_FUNCTION();

		m_LocalData = Buffer::Copy(indices, m_Count * sizeof(uint32_t));
		Renderer::Submit([instance = Ref<OpenGLIndexBuffer>(this)]() mutable {
			glCreateBuffers(1, &instance->m_RendererID);
			glNamedBufferData(instance->m_RendererID, instance->m_Count * sizeof(uint32_t), instance->m_LocalData.Data, GL_STATIC_DRAW);
		});
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		PP_PROFILE_FUNCTION();

		Renderer::Submit([instance = Ref<OpenGLIndexBuffer>(this)]()
		{
			glDeleteBuffers(1, &instance->m_RendererID);
		});
	}

	void OpenGLIndexBuffer::SetData(const void* data, uint32_t count, uint32_t offset)
	{
		m_Count = count;
		m_LocalData = Buffer::Copy(data, m_Count*sizeof(uint32_t));
		Renderer::Submit([instance = Ref<OpenGLIndexBuffer>(this), offset]() {
			glNamedBufferSubData(instance->m_RendererID, offset, instance->m_Count * sizeof(uint32_t), instance->m_LocalData.Data);
		});
	}

	void OpenGLIndexBuffer::Bind() const
	{
		PP_PROFILE_FUNCTION();

		Renderer::Submit([instance = Ref<const OpenGLIndexBuffer>(this)]() {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, instance->m_RendererID);
		});
	}

}

