#include "PPpch.h"
#include "OpenGLVertexArray.h"
#include "OpenGLUtils.h"
#include "Puppet/Renderer/Renderer.h"
#include <glad/glad.h>
namespace Puppet
{

	OpenGLVertexArray::OpenGLVertexArray()
	{
		PP_PROFILE_FUNCTION();

		Ref <OpenGLVertexArray>instance = this;
		Renderer::Submit([instance]()mutable {
			glCreateVertexArrays(1, &instance->m_RendererID);
		});
	}

	OpenGLVertexArray::~OpenGLVertexArray()
	{
		PP_PROFILE_FUNCTION();
		RendererID rid = m_RendererID;
		Renderer::Submit([rid](){
			glDeleteVertexArrays(1, &rid);
			});
	}


	void OpenGLVertexArray::Bind() const
	{
		PP_PROFILE_FUNCTION();

		Ref <const OpenGLVertexArray>instance = this;
		Renderer::Submit([instance]()mutable {
			glBindVertexArray(instance->m_RendererID);
			});
	}

	void OpenGLVertexArray::Unbind() const
	{
		PP_PROFILE_FUNCTION();
		Renderer::Submit([]() {
			glBindVertexArray(0);
			});
	}

	void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
	{
		PP_PROFILE_FUNCTION();

		PP_CORE_ASSERT(!vertexBuffer->GetLayout().GetElements().empty(), "Vertex Buffer has no layout!");

		Bind();
		vertexBuffer->Bind();
		Ref <OpenGLVertexArray>instance = this;
		Renderer::Submit([instance, vertexBuffer]()mutable {
			const auto& layout = vertexBuffer->GetLayout();
			for (const auto& element : layout)
			{
				switch (element.Type)
				{
				case ShaderDataType::Float:
				case ShaderDataType::Float2:
				case ShaderDataType::Float3:
				case ShaderDataType::Float4:
				{
					glEnableVertexAttribArray(instance->m_VertexBufferIndex);
					glVertexAttribPointer(instance->m_VertexBufferIndex,
						element.GetComponentCount(),
						OpenGLUtils::ShaderDataTypeToOpenGLBaseType(element.Type),
						element.Normalized ? GL_TRUE : GL_FALSE,
						layout.GetStride(),
						(const void*)element.Offset);
					instance->m_VertexBufferIndex++;
					break;
				}
				case ShaderDataType::Int:
				case ShaderDataType::Int2:
				case ShaderDataType::Int3:
				case ShaderDataType::Int4:
				case ShaderDataType::Bool:
				{
					glEnableVertexAttribArray(instance->m_VertexBufferIndex);
					glVertexAttribIPointer(instance->m_VertexBufferIndex,
						element.GetComponentCount(),
						OpenGLUtils::ShaderDataTypeToOpenGLBaseType(element.Type),
						layout.GetStride(),
						(const void*)element.Offset);
					instance->m_VertexBufferIndex++;
					break;
				}
				case ShaderDataType::Mat3:
				case ShaderDataType::Mat4:
				{
					uint8_t count = element.GetComponentCount();
					for (uint8_t i = 0; i < count; i++)
					{
						glEnableVertexAttribArray(instance->m_VertexBufferIndex);
						glVertexAttribPointer(instance->m_VertexBufferIndex,
							count,
							OpenGLUtils::ShaderDataTypeToOpenGLBaseType(element.Type),
							element.Normalized ? GL_TRUE : GL_FALSE,
							layout.GetStride(),
							(const void*)(element.Offset + sizeof(float) * count * i));
						glVertexAttribDivisor(instance->m_VertexBufferIndex, 1);
						instance->m_VertexBufferIndex++;
					}
					break;
				}
				default:
					PP_CORE_ASSERT(false, "Unknown ShaderDataType!");
				}
			}
		});

		//const auto& layout = vertexBuffer->GetLayout();
		//for (const auto& element : layout)
		//{
		//	switch (element.Type)
		//	{
		//	case ShaderDataType::Float:
		//	case ShaderDataType::Float2:
		//	case ShaderDataType::Float3:
		//	case ShaderDataType::Float4:
		//	{
		//		glEnableVertexAttribArray(m_VertexBufferIndex);
		//		glVertexAttribPointer(m_VertexBufferIndex,
		//			element.GetComponentCount(),
		//			OpenGLUtils::ShaderDataTypeToOpenGLBaseType(element.Type),
		//			element.Normalized ? GL_TRUE : GL_FALSE,
		//			layout.GetStride(),
		//			(const void*)element.Offset);
		//		m_VertexBufferIndex++;
		//		break;
		//	}
		//	case ShaderDataType::Int:
		//	case ShaderDataType::Int2:
		//	case ShaderDataType::Int3:
		//	case ShaderDataType::Int4:
		//	case ShaderDataType::Bool:
		//	{
		//		glEnableVertexAttribArray(m_VertexBufferIndex);
		//		glVertexAttribIPointer(m_VertexBufferIndex,
		//			element.GetComponentCount(),
		//			OpenGLUtils::ShaderDataTypeToOpenGLBaseType(element.Type),
		//			layout.GetStride(),
		//			(const void*)element.Offset);
		//		m_VertexBufferIndex++;
		//		break;
		//	}
		//	case ShaderDataType::Mat3:
		//	case ShaderDataType::Mat4:
		//	{
		//		uint8_t count = element.GetComponentCount();
		//		for (uint8_t i = 0; i < count; i++)
		//		{
		//			glEnableVertexAttribArray(m_VertexBufferIndex);
		//			glVertexAttribPointer(m_VertexBufferIndex,
		//				count,
		//				OpenGLUtils::ShaderDataTypeToOpenGLBaseType(element.Type),
		//				element.Normalized ? GL_TRUE : GL_FALSE,
		//				layout.GetStride(),
		//				(const void*)(element.Offset + sizeof(float) * count * i));
		//			glVertexAttribDivisor(m_VertexBufferIndex, 1);
		//			m_VertexBufferIndex++;
		//		}
		//		break;
		//	}
		//	default:
		//		PP_CORE_ASSERT(false, "Unknown ShaderDataType!");
		//	}
		//}

		m_VertexBuffers.push_back(vertexBuffer);
	}

	void OpenGLVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
	{
		PP_PROFILE_FUNCTION();

		Bind();
		indexBuffer->Bind();

		m_IndexBuffer = indexBuffer;
	}
}
