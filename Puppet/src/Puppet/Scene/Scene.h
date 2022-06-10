#pragma once
#include <entt/entt.hpp>
#include "Puppet/Core/TimeStep.h"
#include "Puppet/Camera/EditorCamera.h"
namespace Puppet {

	class Entity;//forward declaration

	class Scene {
		friend class Entity;
		friend class SceneHierarchyPanel;
		friend class SceneSerializer;

	public:
		Scene();
		~Scene();
		Entity CreateEntity(const std::string& name = std::string());
		void DestroyEntity(Entity entity);

		void OnRenderRuntime(TimeStep ts);
		void OnRenderEditor(TimeStep ts, const EditorCamera& editorCamera);
		void OnViewportResize(uint32_t width, uint32_t height);

		Entity GetPrimaryCameraEntity();
	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);
	private:
		entt::registry m_Registry;
		uint32_t m_ViewportWidth,m_ViewportHeight;
	};
}
