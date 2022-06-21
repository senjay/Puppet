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
		
		Ref<OpenGLVertexBuffer> instance = this;
		Renderer::Submit([instance]() mutable
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
		Ref<OpenGLVertexBuffer> instance = this;
		Renderer::Submit([instance]() mutable
		{
			glCreateBuffers(1, &instance->m_RendererID);
			glNamedBufferData(instance->m_RendererID, instance->m_Size, instance->m_LocalData.Data, OpenGLUtils::OpenGLVBUsage(instance->m_Usage));
		});
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		PP_PROFILE_FUNCTION();
		RendererID rid = m_RendererID;
		Renderer::Submit([rid]()mutable
		{
			glDeleteBuffers(1, &rid);
		});
	}

	void OpenGLVertexBuffer::Bind() const
	{
		PP_PROFILE_FUNCTION();

		Ref<const OpenGLVertexBuffer> instance = this;
		Renderer::Submit([instance]() {
			glBindBuffer(GL_ARRAY_BUFFER, instance->m_RendererID);
		});
	}


	void OpenGLVertexBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
	{
		PP_PROFILE_FUNCTION();

		m_LocalData = Buffer::Copy(data, size);
		m_Size = size;
		Ref<OpenGLVertexBuffer> instance = this;
		Renderer::Submit([instance, offset]() {
			glNamedBufferSubData(instance->m_RendererID, offset, instance->m_Size, instance->m_LocalData.Data);
		});
	}



	//------------------------------------------------------//
	//-----------------IndexBuffer-------------------------//
	//------------------------------------------------------//
	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t count)
		:m_RendererID(0), m_Count(count)
	{
		Ref<OpenGLIndexBuffer> instance = this;
		Renderer::Submit([instance]() mutable {
			glCreateBuffers(1, &instance->m_RendererID);
			glNamedBufferData(instance->m_RendererID, instance->m_Count * sizeof(uint32_t), nullptr, GL_DYNAMIC_DRAW);
		});
	}

	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t * indices, uint32_t count)
		: m_RendererID(0), m_Count(count)
	{
		PP_PROFILE_FUNCTION();

		m_LocalData = Buffer::Copy(indices, m_Count * sizeof(uint32_t));
		Ref<OpenGLIndexBuffer> instance = this;
		Renderer::Submit([instance]() mutable {
			glCreateBuffers(1, &instance->m_RendererID);
			glNamedBufferData(instance->m_RendererID, instance->m_Count * sizeof(uint32_t), instance->m_LocalData.Data, GL_STATIC_DRAW);
		});
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		PP_PROFILE_FUNCTION();

		RendererID rid = m_RendererID;
		Renderer::Submit([rid]()
		{
			glDeleteBuffers(1, &rid);
		});
	}

	void OpenGLIndexBuffer::SetData(const void* data, uint32_t count, uint32_t offset)
	{
		m_Count = count;
		m_LocalData = Buffer::Copy(data, m_Count*sizeof(uint32_t));
		Ref<OpenGLIndexBuffer> instance = this;
		Renderer::Submit([instance, offset]() {
			glNamedBufferSubData(instance->m_RendererID, offset, instance->m_Count * sizeof(uint32_t), instance->m_LocalData.Data);
		});
	}

	void OpenGLIndexBuffer::Bind() const
	{
		PP_PROFILE_FUNCTION();

		Ref<const OpenGLIndexBuffer> instance = this;
		Renderer::Submit([instance]() {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, instance->m_RendererID);
		});
	}

}

