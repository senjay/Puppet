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
		void OnUpdate(TimeStep st);
	private:
		entt::registry m_Registry;
		
	};
}
