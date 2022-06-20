#pragma once
#include <glm/glm.hpp>
#include "Puppet/Core/BaseSingleton.h"
#include "Puppet/Renderer/UniformBuffer.h"
namespace Puppet {
	struct CameraData
	{
		glm::mat4 ViewProjection;
	};
	class UniformBufferLibrary :public BaseSingleton<UniformBufferLibrary>
	{
		friend class BaseSingleton<UniformBufferLibrary>;
	public:
		void Add(const std::string& name, const Ref<UniformBuffer>& uniformBuffer);
		Ref<UniformBuffer> UniformBufferLibrary::Get(const std::string& name);
		bool UniformBufferLibrary::Exists(const std::string& name) const;
	protected:
		UniformBufferLibrary();
	private:
		std::unordered_map<std::string, Ref<UniformBuffer>> m_UniformBuffers;
	};
}