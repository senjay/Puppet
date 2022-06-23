#include "PPpch.h"
#include "OpenGLPipeline.h"
#include "OpenGLUtils.h"
#include "Puppet/Renderer/Pipeline.h"
#include "Puppet/Renderer/Renderer.h"
#include <glad/glad.h>
namespace Puppet {
	OpenGLPipeline::OpenGLPipeline(const PipelineSpecification& spec)
		: m_Specification(spec)
	{
		Invalidate();
	}
	OpenGLPipeline::~OpenGLPipeline()
	{
		Renderer::Submit([instance=Ref<OpenGLPipeline>(this)]()mutable
			{
				glDeleteVertexArrays(1, &instance->m_VertexArrayRendererID);
			});
	}
	void OpenGLPipeline::Invalidate()
	{
		PP_CORE_ASSERT(m_Specification.Layout.GetElements().size(), "Layout is empty!");

		Renderer::Submit([instance = Ref<OpenGLPipeline>(this)]() mutable
			{
				RendererID& vertexArrayRendererID = instance->m_VertexArrayRendererID;

				if (vertexArrayRendererID)
					glDeleteVertexArrays(1, &vertexArrayRendererID);

				glGenVertexArrays(1, &vertexArrayRendererID);
				glBindVertexArray(vertexArrayRendererID);
				glBindVertexArray(0);
			});
	}
	void OpenGLPipeline::Bind()
	{
		Renderer::Submit([instance = Ref<OpenGLPipeline>(this)]()mutable
			{
				glBindVertexArray(instance->m_VertexArrayRendererID);

				const auto& layout = instance->m_Specification.Layout;

				PP_CORE_ASSERT(!layout.GetElements().empty(), "Vertex Buffer has no layout!");
				
				uint32_t attribIndex = 0;	
				for (const auto& element : layout)
				{
					switch (element.Type)
					{
					case ShaderDataType::Float:
					case ShaderDataType::Float2:
					case ShaderDataType::Float3:
					case ShaderDataType::Float4:
					{
						glEnableVertexAttribArray(attribIndex);
						glVertexAttribPointer(attribIndex,
							element.GetComponentCount(),
							OpenGLUtils::ShaderDataTypeToOpenGLBaseType(element.Type),
							element.Normalized ? GL_TRUE : GL_FALSE,
							layout.GetStride(),
							(const void*)element.Offset);
						attribIndex++;
						break;
					}
					case ShaderDataType::Int:
					case ShaderDataType::Int2:
					case ShaderDataType::Int3:
					case ShaderDataType::Int4:
					case ShaderDataType::Bool:
					{
						glEnableVertexAttribArray(attribIndex);
						glVertexAttribIPointer(attribIndex,
							element.GetComponentCount(),
							OpenGLUtils::ShaderDataTypeToOpenGLBaseType(element.Type),
							layout.GetStride(),
							(const void*)element.Offset);
						attribIndex++;
						break;
					}
					case ShaderDataType::Mat3:
					case ShaderDataType::Mat4:
					{
						uint8_t count = element.GetComponentCount();
						for (uint8_t i = 0; i < count; i++)
						{
							glEnableVertexAttribArray(attribIndex);
							glVertexAttribPointer(attribIndex,
								count,
								OpenGLUtils::ShaderDataTypeToOpenGLBaseType(element.Type),
								element.Normalized ? GL_TRUE : GL_FALSE,
								layout.GetStride(),
								(const void*)(element.Offset + sizeof(float) * count * i));
							glVertexAttribDivisor(attribIndex, 1);
							attribIndex++;
						}
						break;
					}
					default:
						PP_CORE_ASSERT(false, "Unknown ShaderDataType!");
					}
				}
			
			});
	}
}