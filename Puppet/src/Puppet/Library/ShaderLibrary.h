#pragma once
#include "Puppet/Renderer/Shader.h"
#include "Puppet/Core/BaseSingleton.h"
namespace Puppet {

	//should be Singleton?
	class ShaderLibrary:public BaseSingleton<ShaderLibrary>
	{
		friend class BaseSingleton<ShaderLibrary>;
	public:
		void Add(const std::string& name, const Ref<Shader>& shader);
		void Set(const std::string& name, const Ref<Shader>& shader);
		Ref<Shader> Get(const std::string& name);
		bool Exists(const std::string& name) const;
		void Init();
	protected:
		ShaderLibrary();
		Ref<Shader> Load(const std::string& filepath);
		Ref<Shader> Load(const std::string& name, const std::string& filepath);
	private:
		void Add(const Ref<Shader>& shader);
	private:
		std::unordered_map<std::string, Ref<Shader>> m_Shaders;
	};
}

