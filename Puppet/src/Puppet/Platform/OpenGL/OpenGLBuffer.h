#pragma once
#include "Puppet/Renderer/VertexBuffer.h"
#include "Puppet/Renderer/IndexBuffer.h"
#include "Puppet/Renderer/RendererAPI.h"
#include "Puppet/Core/Buffer.h"
#include	<glad/glad.h>
namespace Puppet {

	// OpenGL∂•µ„‰÷»æµƒª∫≥Â¿‡
	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(uint32_t size, VertexBufferUsage usage = VertexBufferUsage::Dynamic);
		OpenGLVertexBuffer(void* data, uint32_t size, VertexBufferUsage usage = VertexBufferUsage::Static);
		virtual ~OpenGLVertexBuffer();

		virtual void Bind() const override;
		virtual void SetData(const void* data, uint32_t size, uint32_t offset=0)override;
		virtual void SetLayout(const BufferLayout& layout) override { m_Layout = layout; };
		virtual const BufferLayout& GetLayout()const  override { return m_Layout; };
	private:
		RendererID m_RendererID;
		BufferLayout  m_Layout;
		uint32_t m_Size;
		VertexBufferUsage m_Usage;
		Buffer m_LocalData;
	};

	// OpenGL∂•µ„µƒ‰÷»æÀ≥–Úª∫≥Â¿‡
	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(uint32_t count);
		OpenGLIndexBuffer(const void* indices, uint32_t count);
		virtual ~OpenGLIndexBuffer();
		virtual void SetData(const void* data, uint32_t count, uint32_t offset = 0)override;
		virtual void Bind() const;

		virtual uint32_t GetCount() const { return m_Count; }

	private:
		RendererID m_RendererID;
		uint32_t m_Count;
		Buffer m_LocalData;
	};
}
