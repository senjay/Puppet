#pragma once
#include "Puppet/Core/Buffer.h"
#include "RendererAPI.h"
#include "ShaderUniform.h"
#include <string>
#include <glm/glm.hpp>
namespace Puppet
{
	class Shader:public RefCounted
	{
	public:
		using ShaderReloadedCallback = std::function<void()>;

		virtual ~Shader() = default;

		virtual void Reload() = 0;
		virtual void AddShaderReloadedCallback(const ShaderReloadedCallback& callback) = 0;

		virtual RendererID GetRendererID()const = 0;

		virtual void Bind() const=0;
		virtual void UnBind() const=0;

		virtual void SetInt(const std::string& name, int value) = 0;
		virtual void SetIntArray(const std::string& name, int* values, uint32_t count) = 0;
		virtual void SetBool(const std::string& name, bool value) = 0;
		virtual void SetFloat(const std::string& name, float value) = 0;
		virtual void SetFloat2(const std::string& name, const glm::vec2& value) = 0;
		virtual void SetFloat3(const std::string& name, const glm::vec3& value) = 0;
		virtual void SetFloat4(const std::string& name, const glm::vec4& value) = 0;
		virtual void SetMat4(const std::string& name, const glm::mat4& value) = 0;
		virtual void SetMat4FromRenderThread(const std::string& name, const glm::mat4& value, bool bind = true) = 0;

		virtual const std::string& GetName()const = 0;

		virtual void SetVSMaterialUniformBuffer(Buffer buffer) = 0;
		virtual void SetPSMaterialUniformBuffer(Buffer buffer) = 0;

		virtual const ShaderUniformBufferList& GetVSRendererUniforms() const = 0;
		virtual const ShaderUniformBufferList& GetPSRendererUniforms() const = 0;
		virtual bool HasVSMaterialUniformBuffer() const = 0;
		virtual bool HasPSMaterialUniformBuffer() const = 0;
		virtual const ShaderUniformBufferDeclaration& GetVSMaterialUniformBuffer() const = 0;
		virtual const ShaderUniformBufferDeclaration& GetPSMaterialUniformBuffer() const = 0;

		virtual const ShaderResourceList& GetResources() const = 0;

		static Ref<Shader> Create(const std::string& glslpath);
	};
}
