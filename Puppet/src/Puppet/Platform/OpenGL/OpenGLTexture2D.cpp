#include "PPpch.h"
#include "OpenGLTexture2D.h"
#include "Puppet/Renderer/Renderer.h"
#include "OpenGLUtils.h"
namespace Puppet {



	OpenGLTexture2D::OpenGLTexture2D(const std::string& path):m_Path(path)
	{
		PP_PROFILE_FUNCTION();
		

			int width, height, channels;
			//stbi_set_flip_vertically_on_load(1);
			{
				PP_PROFILE_SCOPE("OpenGLTexture2D::OpenGLTexture2D(const std::string& path)::stbi_load");
				m_ImageData.Data = stbi_load(m_Path.c_str(), &width, &height, &channels, 0);
			}
			PP_CORE_ASSERT(m_ImageData.Data, "Failed to load image!");

			if (m_ImageData.Data)
			{
				m_IsLoaded = true;

				m_Width = width;
				m_Height = height;

				GLenum internalFormat = 0, dataFormat = 0;
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
				m_InternalFormat = internalFormat;
				m_DataFormat = dataFormat;

				PP_CORE_ASSERT(internalFormat && dataFormat, "Picture format channel not supported!");

				Ref<OpenGLTexture2D> instance = this;
				Renderer::Submit([instance]() mutable {
					glCreateTextures(GL_TEXTURE_2D, 1, &instance->m_RendererID);
					//internalFormat:how opengl storate texture 
					glTextureStorage2D(instance->m_RendererID, 1, instance->m_InternalFormat, instance->m_Width, instance->m_Height);

					glTextureParameteri(instance->m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTextureParameteri(instance->m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

					//GL_TEXTURE_WRAP_S:X方向;GL_TEXTURE_WRAP_T:Y方向;
					glTextureParameteri(instance->m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
					glTextureParameteri(instance->m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
					//dataFormat:what dataFormat our picture 
					glTextureSubImage2D(instance->m_RendererID, 0, 0, 0, instance->m_Width, instance->m_Height, instance->m_DataFormat, GL_UNSIGNED_BYTE, instance->m_ImageData.Data);

					stbi_image_free(instance->m_ImageData.Data);
					});
			
			}
	}

	OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height)
		:m_Width(width),m_Height(height)
	{
		PP_PROFILE_FUNCTION();

		m_InternalFormat = GL_RGBA8;
		m_DataFormat = GL_RGBA;

		Ref<OpenGLTexture2D> instance = this;
		Renderer::Submit([instance]() mutable {
			glCreateTextures(GL_TEXTURE_2D, 1, &instance->m_RendererID);
			//internalFormat:how opengl storate texture 
			glTextureStorage2D(instance->m_RendererID, 1, instance->m_InternalFormat, instance->m_Width, instance->m_Height);

			glTextureParameteri(instance->m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameteri(instance->m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			//GL_TEXTURE_WRAP_S:X方向;GL_TEXTURE_WRAP_T:Y方向;
			glTextureParameteri(instance->m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTextureParameteri(instance->m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
		});
		/*
			如果在构造函数调用this构造侵入式指针，出了构造函数，该指针被delete
			导致这样一个情况：该对象的指针在构造函数创建后被立马销毁，即外部无法获取该对象指针
		bug触发条件：【回调函数在该构造函数未完成时就被调用】
			当运行到此处，此时共有2个引用计数，一个在函数，一个在回调函数中，
			如果回调函数中的立马被消费掉，则只剩一个引用计数，
			而出了该构造函数，构造函数中的智能指针也析构，导致引用计数为0，则该对象直接被delete
		*/
		m_ImageData.Allocate(width * height * Texture::GetBPP(TextureFormat::RGBA));
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		PP_PROFILE_FUNCTION();
		RendererID rid = m_RendererID;
		Renderer::Submit([rid](){
			glDeleteTextures(1, &rid);
		});
	}

	void OpenGLTexture2D::SetData(void* data, uint32_t size)
	{
		PP_PROFILE_FUNCTION();

		uint32_t bpp = m_DataFormat == GL_RGBA ? 4 : 3;
		PP_CORE_ASSERT(size == m_Width * m_Height * bpp, "Data must be entire texture!");
		m_ImageData.Write(data, size);
		
		Ref<OpenGLTexture2D> instance = this;
		Renderer::Submit([instance]() mutable {
			glTextureSubImage2D(instance->m_RendererID, 0, 0, 0, instance->m_Width, instance->m_Height, instance->m_DataFormat, GL_UNSIGNED_BYTE,instance->m_ImageData.Data);
		});
		
	}

	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		PP_PROFILE_FUNCTION();
		Ref<const OpenGLTexture2D> instance = this;
		Renderer::Submit([instance, slot]() {
			glBindTextureUnit(slot, instance->m_RendererID);
		});
	}

}
