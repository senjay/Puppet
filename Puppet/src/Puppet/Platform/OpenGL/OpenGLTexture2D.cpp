#include "PPpch.h"
#include "OpenGLTexture2D.h"
#include "Puppet/Renderer/Renderer.h"
#include "OpenGLUtils.h"
namespace Puppet {

	static GLenum PuppetToOpenGLTextureFormat(TextureFormat format)
	{
		switch (format)
		{
		case Puppet::TextureFormat::RGB:     return GL_RGB;
		case Puppet::TextureFormat::RGBA:    return GL_RGBA;
		case Puppet::TextureFormat::Float16: return GL_RGBA16F;
		}
		PP_CORE_ASSERT(false, "Unknown texture format!");
		return 0;
	}

	//------------------------------------------------------//
	//-----------------OpenGLTexture2D----------------------//
	//------------------------------------------------------//
	OpenGLTexture2D::OpenGLTexture2D(TextureFormat format, uint32_t width, uint32_t height, TextureWrap wrap)
		:m_Format(format), m_Width(width), m_Height(height), m_Wrap(wrap)
	{
		/*
			����ڹ��캯������this��������ʽָ�룬���˹��캯������ָ�뱻delete
			��������һ��������ö����ָ���ڹ��캯���������������٣����ⲿ�޷���ȡ�ö���ָ��
		bug�������������ص������ڸù��캯��δ���ʱ�ͱ����á�
			�����е��˴�����ʱ����2�����ü�����һ���ں�����һ���ڻص������У�
			����ص������е��������ѵ�����ֻʣһ�����ü�����
			�����˸ù��캯�������캯���е�����ָ��Ҳ�������������ü���Ϊ0����ö���ֱ�ӱ�delete
		*/
		Ref<OpenGLTexture2D> instance = this;
		Renderer::Submit([instance]() mutable
			{
				//glCreateTextures(GL_TEXTURE_2D, 1, &instance->m_RendererID);
				glGenTextures(1, &instance->m_RendererID);
				glBindTexture(GL_TEXTURE_2D, instance->m_RendererID);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				GLenum wrap = instance->m_Wrap == TextureWrap::Clamp ? GL_CLAMP_TO_EDGE : GL_REPEAT;
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
				glTextureParameterf(instance->m_RendererID, GL_TEXTURE_MAX_ANISOTROPY, RenderCommand::GetCapabilities().MaxAnisotropy);

				glTexImage2D(GL_TEXTURE_2D, 0, PuppetToOpenGLTextureFormat(instance->m_Format), instance->m_Width, instance->m_Height, 0, PuppetToOpenGLTextureFormat(instance->m_Format), GL_UNSIGNED_BYTE, nullptr);

				glBindTexture(GL_TEXTURE_2D, 0);
			});

		m_ImageData.Allocate(width * height * Texture::GetBPP(m_Format));
	}

	OpenGLTexture2D::OpenGLTexture2D(const std::string& path, bool srgb)
		:m_Path(path)
	{
		int width, height, channels;
		if (stbi_is_hdr(path.c_str()))
		{
			PP_CORE_INFO("Loading HDR texture {0}, srgb={1}", path, srgb);
			m_ImageData.Data = (byte*)stbi_loadf(path.c_str(), &width, &height, &channels, 0);
			m_IsHDR = true;
			m_Format = TextureFormat::Float16;
		}
		else
		{
			PP_CORE_INFO("Loading texture {0}, srgb={1}", path, srgb);
			m_ImageData.Data = stbi_load(path.c_str(), &width, &height, &channels, srgb ? STBI_rgb : STBI_rgb_alpha);
			PP_CORE_ASSERT(m_ImageData.Data, "Could not read image!");
			m_Format = TextureFormat::RGBA;
		}

		if (!m_ImageData.Data)
			return;

		m_IsLoaded = true;

		m_Width = width;
		m_Height = height;

		Renderer::Submit([instance= Ref<OpenGLTexture2D>(this), srgb]() mutable
			{
				// TODO: Consolidate properly
				if (srgb)
				{
					glCreateTextures(GL_TEXTURE_2D, 1, &instance->m_RendererID);
					int levels = Texture::CalculateMipMapCount(instance->m_Width, instance->m_Height);
					glTextureStorage2D(instance->m_RendererID, levels, GL_SRGB8, instance->m_Width, instance->m_Height);
					glTextureParameteri(instance->m_RendererID, GL_TEXTURE_MIN_FILTER, levels > 1 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
					glTextureParameteri(instance->m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

					glTextureSubImage2D(instance->m_RendererID, 0, 0, 0, instance->m_Width, instance->m_Height, GL_RGB, GL_UNSIGNED_BYTE, instance->m_ImageData.Data);
					glGenerateTextureMipmap(instance->m_RendererID);
				}
				else
				{
					glGenTextures(1, &instance->m_RendererID);
					glBindTexture(GL_TEXTURE_2D, instance->m_RendererID);

					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

					GLenum internalFormat = PuppetToOpenGLTextureFormat(instance->m_Format);
					GLenum format = srgb ? GL_SRGB8 : (instance->m_IsHDR ? GL_RGB : PuppetToOpenGLTextureFormat(instance->m_Format)); // HDR = GL_RGB for now
					GLenum type = internalFormat == GL_RGBA16F ? GL_FLOAT : GL_UNSIGNED_BYTE;
					glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, instance->m_Width, instance->m_Height, 0, format, type, instance->m_ImageData.Data);
					glGenerateMipmap(GL_TEXTURE_2D);

					glBindTexture(GL_TEXTURE_2D, 0);
				}
				stbi_image_free(instance->m_ImageData.Data);
			});
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		PP_PROFILE_FUNCTION();

		Renderer::Submit([instance = Ref<OpenGLTexture2D>(this)](){
			glDeleteTextures(1, &instance->m_RendererID);
		});
	}

	uint32_t OpenGLTexture2D::GetMipLevelCount() const
	{
		return Texture::CalculateMipMapCount(m_Width, m_Height);
	}

	void OpenGLTexture2D::SetData(void* data, uint32_t size)
	{
		PP_PROFILE_FUNCTION();

		PP_CORE_ASSERT(size == m_Width * m_Height * GetBPP(m_Format), "Data must be entire texture!");
		m_ImageData.Write(data, size);
		
		Renderer::Submit([instance = Ref<OpenGLTexture2D>(this)]() mutable {
			glTextureSubImage2D(instance->m_RendererID, 0, 0, 0, instance->m_Width, instance->m_Height, PuppetToOpenGLTextureFormat(instance->m_Format), GL_UNSIGNED_BYTE, instance->m_ImageData.Data);
		});
		
	}

	void OpenGLTexture2D::Resize(uint32_t width, uint32_t height)
	{
		m_ImageData.Allocate(width * height * Texture::GetBPP(m_Format));
	}

	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		PP_PROFILE_FUNCTION();

		Renderer::Submit([instance=Ref<const OpenGLTexture2D>(this),slot]() {
			glBindTextureUnit(slot, instance->m_RendererID);
		});
	}



	//------------------------------------------------------//
	//-----------------TextureCube-------------------------//
	//------------------------------------------------------//
	OpenGLTextureCube::OpenGLTextureCube(TextureFormat format, uint32_t width, uint32_t height)
		:m_Width(width),m_Height(height),m_Format(format)
	{

		uint32_t levels = Texture::CalculateMipMapCount(width, height);
		
		Renderer::Submit([instance = Ref<OpenGLTextureCube>(this), levels]() mutable
			{
				glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &instance->m_RendererID);
				glTextureStorage2D(instance->m_RendererID, levels, PuppetToOpenGLTextureFormat(instance->m_Format), instance->m_Width, instance->m_Height);
				glTextureParameteri(instance->m_RendererID, GL_TEXTURE_MIN_FILTER, levels > 1 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
				glTextureParameteri(instance->m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			});
	}

	OpenGLTextureCube::OpenGLTextureCube(const std::string& path)
		:m_Path(path)
	{
		int width, height, channels;
		stbi_set_flip_vertically_on_load(false);
		m_ImageData.Data=stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb);

		m_Width = width;
		m_Height = height;
		m_Format = TextureFormat::RGB;

		uint32_t faceWidth = m_Width / 4;
		uint32_t faceHeight = m_Height / 3;
		PP_CORE_ASSERT(faceWidth == faceHeight, "Non-square faces!");

		std::array<uint8_t*, 6> faces;
		for (size_t i = 0; i < faces.size(); i++)
			faces[i] = new uint8_t[faceWidth * faceHeight * 3]; // 3 BPP

		int faceIndex = 0;

		for (size_t i = 0; i < 4; i++)
		{
			for (size_t y = 0; y < faceHeight; y++)
			{
				size_t yOffset = y + faceHeight;
				for (size_t x = 0; x < faceWidth; x++)
				{
					size_t xOffset = x + i * faceWidth;
					faces[faceIndex][(x + y * faceWidth) * 3 + 0] = m_ImageData[(xOffset + yOffset * m_Width) * 3 + 0];
					faces[faceIndex][(x + y * faceWidth) * 3 + 1] = m_ImageData[(xOffset + yOffset * m_Width) * 3 + 1];
					faces[faceIndex][(x + y * faceWidth) * 3 + 2] = m_ImageData[(xOffset + yOffset * m_Width) * 3 + 2];
				}
			}
			faceIndex++;
		}

		for (size_t i = 0; i < 3; i++)
		{
			// Skip the middle one
			if (i == 1)
				continue;

			for (size_t y = 0; y < faceHeight; y++)
			{
				size_t yOffset = y + i * faceHeight;
				for (size_t x = 0; x < faceWidth; x++)
				{
					size_t xOffset = x + faceWidth;
					faces[faceIndex][(x + y * faceWidth) * 3 + 0] = m_ImageData[(xOffset + yOffset * m_Width) * 3 + 0];
					faces[faceIndex][(x + y * faceWidth) * 3 + 1] = m_ImageData[(xOffset + yOffset * m_Width) * 3 + 1];
					faces[faceIndex][(x + y * faceWidth) * 3 + 2] = m_ImageData[(xOffset + yOffset * m_Width) * 3 + 2];
				}
			}
			faceIndex++;
		}

		
		Renderer::Submit([instance = Ref<OpenGLTextureCube>(this), faceWidth, faceHeight, faces]() mutable
			{
				glGenTextures(1, &instance->m_RendererID);
				glBindTexture(GL_TEXTURE_CUBE_MAP, instance->m_RendererID);

				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTextureParameterf(instance->m_RendererID, GL_TEXTURE_MAX_ANISOTROPY, RenderCommand::GetCapabilities().MaxAnisotropy);

				auto format = PuppetToOpenGLTextureFormat(instance->m_Format);
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, format, faceWidth, faceHeight, 0, format, GL_UNSIGNED_BYTE, faces[2]);
				glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, format, faceWidth, faceHeight, 0, format, GL_UNSIGNED_BYTE, faces[0]);

				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, format, faceWidth, faceHeight, 0, format, GL_UNSIGNED_BYTE, faces[4]);
				glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, format, faceWidth, faceHeight, 0, format, GL_UNSIGNED_BYTE, faces[5]);

				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, format, faceWidth, faceHeight, 0, format, GL_UNSIGNED_BYTE, faces[1]);
				glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, format, faceWidth, faceHeight, 0, format, GL_UNSIGNED_BYTE, faces[3]);

				glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

				glBindTexture(GL_TEXTURE_2D, 0);

				for (size_t i = 0; i < faces.size(); i++)
					delete[] faces[i];

				stbi_image_free(instance->m_ImageData.Data);
			});
	}

	OpenGLTextureCube::~OpenGLTextureCube()
	{
		Renderer::Submit([instance = Ref<OpenGLTextureCube>(this)]() {
			glDeleteTextures(1, &instance->m_RendererID);
		});
	}

	void OpenGLTextureCube::Bind(uint32_t slot) const
	{
		Renderer::Submit([instance = Ref<const OpenGLTextureCube>(this),slot]() {
			glBindTextureUnit(slot, instance->m_RendererID);
		});
	}

	uint32_t OpenGLTextureCube::GetMipLevelCount() const
	{
		return Texture::CalculateMipMapCount(m_Width, m_Height);
	}

}
