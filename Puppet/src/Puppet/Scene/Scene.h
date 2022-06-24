#pragma once
#include <entt/entt.hpp>
#include "Puppet/Core/TimeStep.h"
#include "Puppet/Camera/EditorCamera.h"
#include "Puppet/Core/UUID.h"
#include "Puppet/Renderer/SceneEnvironment.h"
#include "Puppet/Library/ShaderLibrary.h"
#include "Puppet/Library/TextureLibrary.h"
#include "Puppet/Renderer/Material.h"
namespace Puppet {
	struct Light
	{
		glm::vec3 Direction = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Radiance = { 0.0f, 0.0f, 0.0f };

		float Multiplier = 1.0f;
	};

	struct DirectionalLight
	{
		glm::vec3 Direction = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Radiance = { 0.0f, 0.0f, 0.0f };
		float Multiplier = 0.0f;

		// C++ only
		bool CastShadows = true;
	};

	struct LightEnvironment
	{
		DirectionalLight DirectionalLights[4];
	};

	class Entity;//forward declaration
	using EntityMap = std::unordered_map<UUID, Entity>;

	class Scene :public RefCounted
	{
		friend class Entity;
		friend class SceneHierarchyPanel;
		friend class SceneSerializer;
		friend class SceneRenderer;
	public:
		Scene();
		~Scene();
		void Init();
		
		void OnUpdate(TimeStep ts){}
		void OnRenderRuntime(TimeStep ts);
		void OnRenderEditor(TimeStep ts, const EditorCamera& editorCamera);
		void OnEvent(Event& e){}
		void OnViewportResize(uint32_t width, uint32_t height);
		// Runtime
		void OnRuntimeStart() { m_IsPlaying = true; }
		void OnRuntimeStop() {m_IsPlaying = false; }

		
		const Environment& GetEnvironment() const { return m_Environment; }
		void SetSkybox(const Ref<TextureCube>& skybox);

		Light& GetLight() { return m_Light; }
		const Light& GetLight() const { return m_Light; }
		float& GetSkyboxLod() { return m_SkyboxLod; }

		Entity CreateEntity(const std::string& name = std::string());
		Entity CreateEntityWithID(UUID uuid, const std::string& name = std::string(), bool runtimeMap = false);
		void DestroyEntity(Entity entity);

		Entity GetPrimaryCameraEntity();
		std::string GetSceneName() { return m_SceneName; }
		const EntityMap& GetEntityMap() const { return m_EntityIDMap; }
		UUID GetUUID() const { return m_SceneID; }
		template<typename T>
		auto GetAllEntitiesWith()
		{
			return m_Registry.view<T>();
		}
		
		void SetSelectEntity(entt::entity entity) { m_SelectedEntity = entity; }
	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);
	private:
		UUID m_SceneID;
		entt::registry m_Registry;
		uint32_t m_ViewportWidth,m_ViewportHeight;
		std::string m_SceneName="Untitled";
		entt::entity m_SceneEntity;
		EntityMap m_EntityIDMap;
		Light m_Light;
		float m_LightMultiplier = 0.3f;
		LightEnvironment m_LightEnvironment;
		Environment m_Environment;
		float m_EnvironmentIntensity = 1.0f;
		Ref<TextureCube> m_SkyboxTexture;
		Ref<MaterialInstance> m_SkyboxMaterial;
		entt::entity m_SelectedEntity;
		Entity* m_PhysicsBodyEntityBuffer = nullptr;
		float m_SkyboxLod = 1.0f;
		bool m_IsPlaying = false;
	};
}
