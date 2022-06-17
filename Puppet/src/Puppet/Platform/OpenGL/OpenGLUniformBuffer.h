#pragma once
#include "Puppet/Core/Buffer.h"
#include "Puppet/Renderer/UniformBuffer.h"
#include "Puppet/Renderer/RendererAPI.h"
namespace Puppet {

	class OpenGLUniformBuffer : public UniformBuffer
	{
	public:
		OpenGLUniformBuffer(uint32_t size, uint32_t binding);
		virtual ~OpenGLUniformBuffer();

		virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) override;
	private:
		RendererID m_RendererID = 0;
		Buffer m_LocalData;
		uint32_t m_Size;
	};
}

