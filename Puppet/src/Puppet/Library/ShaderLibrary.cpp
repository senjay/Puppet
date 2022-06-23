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
		Load("BloomBlur", "./assets/shaders/gl/BloomBlur.glsl");
		Load("BloomBlend", "./assets/shaders/gl/BloomBlend.glsl");
		Load("EnvironmentIrradiance", "./assets/shaders/gl/EnvironmentIrradiance.glsl");
		Load("EnvironmentMipFilter", "./assets/shaders/gl/EnvironmentMipFilter.glsl");
		Load("EquirectangularToCubeMap", "./assets/shaders/gl/EquirectangularToCubeMap.glsl");
		Load("Grid", "./assets/shaders/gl/Grid.glsl");
		Load("HazelPBR_Anim", "./assets/shaders/gl/HazelPBR_Anim.glsl");
		Load("HazelPBR_Static", "./assets/shaders/gl/HazelPBR_Static.glsl");
		Load("Outline", "./assets/shaders/gl/Outline.glsl");
		Load("Renderer2D", "./assets/shaders/gl/Renderer2D.glsl");
		Load("Renderer2D_Line", "./assets/shaders/gl/Renderer2D_Line.glsl");
		Load("SceneComposite", "./assets/shaders/gl/SceneComposite.glsl");
		Load("shader", "./assets/shaders/gl/shader.glsl");
		Load("ShadowMap", "./assets/shaders/gl/ShadowMap.glsl");
		Load("Skybox", "./assets/shaders/gl/Skybox.glsl");
	}
}