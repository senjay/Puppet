#pragma once
#include "Puppet/Renderer/Texture.h"
#include "Puppet/Core/Buffer.h"
#include "stb_image.h"
#include <glad/glad.h>
namespace Puppet {

	class OpenGLTexture2D:public Texture2D
	{
	public:
		OpenGLTexture2D(TextureFormat format, uint32_t width, uint32_t height, TextureWrap wrap);
		OpenGLTexture2D(const std::string& path, bool srgb);
		~OpenGLTexture2D();

		uint32_t GetWidth() const override { return m_Width; };
		uint32_t GetHeight() const override { return m_Height; };
		RendererID GetRendererID() const override { return m_RendererID; };
		virtual uint32_t GetMipLevelCount() const override;
		virtual const std::string& GetPath() const override { return m_Path; }

		bool IsLoaded() const override { return m_IsLoaded; };
		void SetData(void* data, uint32_t size)override;

		void Bind(uint32_t slot = 0)const override;

		bool operator==(const Texture& other) const override 
		{ 
			return m_RendererID== ((OpenGLTexture2D&)other).m_RendererID;
		};
		
	private:
		std::string m_Path;
		uint32_t m_Width, m_Height;
		uint32_t m_RendererID;
		TextureFormat m_Format;
		TextureWrap m_Wrap = TextureWrap::Clamp;
		bool m_IsLoaded = false;
		bool m_IsHDR = false;

		//TODO: Buffer write not thread safe
		Buffer m_ImageData;
	};

	class OpenGLTextureCube : public TextureCube
	{
	public:
		OpenGLTextureCube(TextureFormat format, uint32_t width, uint32_t height);
		OpenGLTextureCube(const std::string& path);
		virtual ~OpenGLTextureCube();

		virtual void Bind(uint32_t slot = 0) const;

		virtual TextureFormat GetFormat() const { return m_Format; }
		virtual uint32_t GetWidth() const { return m_Width; }
		virtual uint32_t GetHeight() const { return m_Height; }
		virtual uint32_t GetMipLevelCount() const override;
		virtual const std::string& GetPath() const override { return m_Path; }
		virtual RendererID GetRendererID() const override { return m_RendererID; }

		virtual bool operator==(const Texture& other) const override
		{
			return m_RendererID == ((OpenGLTextureCube&)other).m_RendererID;
		}
	private:
		RendererID m_RendererID;
		std::string m_Path;
		TextureFormat m_Format;
		uint32_t m_Width, m_Height;

		Buffer m_ImageData;
	};
}

