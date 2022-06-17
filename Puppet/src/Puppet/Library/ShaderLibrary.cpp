#include "PPpch.h"
#include "ShaderLibrary.h"
namespace Puppet {
	ShaderLibrary::ShaderLibrary()
	{

	}
	void ShaderLibrary::Add(const std::string& name, const Ref<Shader>& shader)
	{
		PP_CORE_ASSERT(!Exists(name), "Shader already exists!");
		m_Shaders[name] = shader;
	}

	void ShaderLibrary::Set(const std::string& name, const Ref<Shader>& shader)
	{
		PP_CORE_ASSERT(Exists(name), "Can't find this Shader in Library!");
		m_Shaders[name] = shader;
	}

	void ShaderLibrary::Add(const Ref<Shader>& shader)
	{
		auto& name = shader->GetName();
		Add(name, shader);
	}

	Ref<Shader> ShaderLibrary::Load(const std::string& filepath)
	{
		auto shader = Shader::Create(filepath);
		Add(shader);
		return shader;
	}
	Ref<Shader> ShaderLibrary::Load(const std::string& name, const std::string& filepath)
	{
		auto shader = Shader::Create(filepath);
		Add(name, shader);
		return shader;
	}
	Ref<Shader> ShaderLibrary::Get(const std::string& name)
	{
		PP_CORE_ASSERT(Exists(name), "Shader not found!");
		return m_Shaders[name];
	}

	bool ShaderLibrary::Exists(const std::string& name) const
	{
		return m_Shaders.find(name) != m_Shaders.end();
	}
	void ShaderLibrary::Init()
	{
		Load("Texture2D", "./assets/shaders/Texture2D.glsl");
		Load("Texture3D", "./assets/shaders/Texture3D.glsl");
	}
}