#include "PPpch.h"
#include "OpenGLTexture2D.h"
#include <glad/glad.h>
namespace Puppet {



	OpenGLTexture2D::OpenGLTexture2D(const std::string& path):m_Path(path)
	{
		int width, height, channels;
		//stbi_set_flip_vertically_on_load(1);
		stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		PP_CORE_ASSERT(data, "Failed to load image!");
		m_Width = width;
		m_Height = height;

		GLenum internalFormat = 0, dataFormat;
		if (channels == 4)
		{
			internalFormat = GL_RGBA8;
			dataFormat = GL_RGBA;
		}
		else if (channels == 3)
		{
			internalFormat = GL_RGB8;
			dataFormat = GL_RGB;
		}
		PP_CORE_ASSERT(internalFormat && dataFormat, "Picture format channel not supported!");

		glCreateTextures(GL_TEXTURE_2D,1,&m_RendererID);
		//internalFormat:how opengl storate texture 
		glTextureStorage2D(m_RendererID, 1, internalFormat, m_Width, m_Height);
		
		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		//dataFormat:what dataFormat our picture 
		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, (const void*)data);
		
		stbi_image_free(data); 
	}

	OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height):m_Width(width),m_Height(height)
	{

	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		glBindTextureUnit(slot, m_RendererID);
	}

}
