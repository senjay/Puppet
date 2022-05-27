#pragma once
#include "Puppet/Renderer/Texture.h"
#include "stb_image.h"
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

		void SetData(void* data, uint32_t size)override {};

		void Bind(uint32_t slot = 0)const override;

		bool IsLoaded() const override { return true; };

		bool operator==(const Texture& other) const override { return true; };
		
	private:
		std::string m_Path;
		uint32_t m_Width, m_Height;
		uint32_t m_RendererID;
	};

}

