#pragma once
#include <entt/entt.hpp>
#include "Puppet/Core/TimeStep.h"
namespace Puppet {

	class Entity;//forward declaration

	class Scene {
		friend class Entity;

	public:
		Scene();
		~Scene();
		Entity CreateEntity(const std::string& name = std::string());
		void OnViewportResize(uint32_t width, uint32_t height);
		void OnUpdate(TimeStep st);
	private:
		entt::registry m_Registry;
		uint32_t m_ViewportWidth,m_ViewportHeight;
	};
}
