#pragma once
#include <entt/entt.hpp>
#include "Components.h"
#include "Puppet/Core/TimeStep.h"
#include "Puppet/Renderer/Renderer.h"
#include "Puppet/Renderer/Renderer2D.h"
namespace Puppet {

	class Scene {
	public:
		Scene();
		~Scene();
		entt::entity CreateEntity(const std::string& name = std::string());
		entt::registry& Reg() { return m_Registry; }
		void OnUpdate(TimeStep st);
	private:
		entt::registry m_Registry;
	};
}
