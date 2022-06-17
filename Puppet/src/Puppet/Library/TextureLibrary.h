#pragma once
#include "Puppet/Renderer/Texture.h"
#include "Puppet/Core/BaseSingleton.h"
namespace Puppet {
	class TextureLibrary :public BaseSingleton<TextureLibrary>
	{
		friend class BaseSingleton<TextureLibrary>;
	public:
		void Add(const std::string& name, const Ref<Texture2D>& texture);
		void Set(const std::string& name, const Ref<Texture2D>& texture);
		Ref<Texture2D> Get(const std::string& name);
		bool Exists(const std::string& name) const;
		void Init();
	protected:
		TextureLibrary();
	private:
		std::unordered_map<std::string, Ref<Texture2D>> m_Textures;
	};
}