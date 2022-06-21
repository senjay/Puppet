#include "PPpch.h"
#include "TextureLibrary.h"
namespace Puppet {
	TextureLibrary::TextureLibrary()
	{
	}
	void TextureLibrary::Add(const std::string& name, const Ref<Texture2D>& texture)
	{
		PP_CORE_ASSERT(!Exists(name), "texture already exists!");
		m_Textures[name] = texture;
	}
	Ref<Texture2D> TextureLibrary::FindorAdd(const std::string& name, const std::string& filepath)
	{
		if (Exists(name))
		{
			return m_Textures[name];
		}
		else
		Add(name, Texture2D::Create(filepath));
		return m_Textures[name];
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
	Ref<Texture2D> TextureLibrary::GetWhiteTexture()
	{
		PP_CORE_ASSERT(Exists("WhiteTexture"), "WhiteTexture not found!");
		return m_Textures["WhiteTexture"];
	}
	bool TextureLibrary::Exists(const std::string& name) const
	{
		return m_Textures.find(name) != m_Textures.end();
	}
	void TextureLibrary::Init()
	{
		Add("DefaultTexture", Texture2D::Create("./assets/textures/DefaultTexture.png"));
		Add("DefaultNormal", Texture2D::Create("./assets/textures/DefaultNormal.png"));
		Add("DefaultMetallicRoughness", Texture2D::Create("./assets/textures/black.png"));
		Add("DefaultHdr", Texture2D::Create("./assets/textures/DefaultTexture.png"));
		Add("BRDF_LUT", Texture2D::Create("./assets/textures/BRDF_LUT.tga"));

		Ref<Texture2D>WhiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));
		Add("WhiteTexture", WhiteTexture);

		Ref<Texture2D> BlackTexture = Texture2D::Create(1, 1);
		uint32_t blackTextureData = 0x00000000;
		BlackTexture->SetData(&blackTextureData, sizeof(uint32_t));
		Add("BlackTexture", BlackTexture);
	}
}