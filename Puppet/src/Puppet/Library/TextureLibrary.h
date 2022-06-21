#pragma once
#include "Puppet/Renderer/Texture.h"
#include "Puppet/Core/BaseSingleton.h"
namespace Puppet {
	class TextureLibrary :public BaseSingleton<TextureLibrary>
	{
		friend class BaseSingleton<TextureLibrary>;
	public:
		void Add(const std::string& name, const Ref<Texture2D>& texture);
		Ref<Texture2D> FindorAdd(const std::string& name, const std::string& filepath);
		void Set(const std::string& name, const Ref<Texture2D>& texture);
		Ref<Texture2D> Get(const std::string& name);
		Ref<Texture2D> GetWhiteTexture();
		bool Exists(const std::string& name) const;
		void Init();
	protected:
		TextureLibrary();
	private:
		std::unordered_map<std::string, Ref<Texture2D>> m_Textures;
	};
}