#pragma once
#include "Puppet/Renderer/Texture.h"
#include "Puppet/Core/Buffer.h"
#include "stb_image.h"
#include <glad/glad.h>
namespace Puppet {
	class OpenGLTexture2D:public Texture2D
	{
	public:
		OpenGLTexture2D(const std::string& path);
		OpenGLTexture2D(uint32_t width, uint32_t height);
		~OpenGLTexture2D();
		uint32_t GetWidth() const override { return m_Width; };
		uint32_t GetHeight() const override { return m_Height; };
		uint32_t GetRendererID() const override { return m_RendererID; };

		void SetData(void* data, uint32_t size)override;

		void Bind(uint32_t slot = 0)const override;

		bool IsLoaded() const override { return m_IsLoaded; };

		bool operator==(const Texture& other) const override 
		{ 
			return m_RendererID== other.GetRendererID();
		};
		
	private:
		std::string m_Path;
		uint32_t m_Width, m_Height;
		uint32_t m_RendererID;
		GLenum m_InternalFormat, m_DataFormat;
		bool m_IsLoaded = false;
		Buffer m_ImageData;
	};

}

