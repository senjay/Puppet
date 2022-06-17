#include "PPpch.h"
#include "TextureLibrary.h"
namespace Puppet {
	TextureLibrary::TextureLibrary()
	{
	}
	void TextureLibrary::Add(const std::string& name, const Ref<Texture2D>& texture)
	{
		PP_CORE_ASSERT(!Exists(name), "Shader already exists!");
		m_Textures[name] = texture;
	}
	void TextureLibrary::Set(const std::string& name, const Ref<Texture2D>& texture)
	{
		PP_CORE_ASSERT(Exists(name), "Can't find this Shader in Library!");
		m_Textures[name] = texture;
	}
	Ref<Texture2D> TextureLibrary::Get(const std::string& name)
	{
		PP_CORE_ASSERT(Exists(name), "Textures not found!");
		return m_Textures[name];
	}
	bool TextureLibrary::Exists(const std::string& name) const
	{
		return m_Textures.find(name) != m_Textures.end();
	}
	void TextureLibrary::Init()
	{
		Add("Logo", Texture2D::Create("./assets/textures/Logo.png"));


		Ref<Texture2D>WhiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));
		Add("WhiteTexture", WhiteTexture);
	}
}