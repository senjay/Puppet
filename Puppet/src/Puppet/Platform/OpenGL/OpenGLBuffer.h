#pragma once
#include "Puppet/Renderer/Buffer.h"
#include	<glad/glad.h>
namespace Puppet {

	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
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

	// OpenGL∂•µ„‰÷»æµƒª∫≥Â¿‡
	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(float* vertices, uint32_t size);
		virtual ~OpenGLVertexBuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;
		virtual void SetLayout(const BufferLayout& layout) override { m_Layout = layout; };
		virtual const BufferLayout& GetLayout()const  override { return m_Layout; };
	private:
		uint32_t m_RendererID;
		BufferLayout  m_Layout;
	};

	// OpenGL∂•µ„µƒ‰÷»æÀ≥–Úª∫≥Â¿‡
	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(uint32_t* indices, uint32_t count);
		virtual ~OpenGLIndexBuffer();

		virtual void Bind() const;
		virtual void Unbind() const;

		virtual uint32_t GetCount() const { return m_Count; }

	private:
		uint32_t m_RendererID;
		uint32_t m_Count;
	};
}
