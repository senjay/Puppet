#include "PPpch.h"
#include "Scene.h"
#include "Entity.h"
#include "Components.h"
#include "Puppet/Renderer/Renderer.h"
#include "Puppet/Renderer/Renderer2D.h"
#include "Puppet/Renderer/SceneRenderer.h"
#include <glm/glm.hpp>
namespace Puppet {

    Scene::Scene()
    {

    }

    Scene::~Scene()
    {
        m_Registry.clear();
    }

    void Scene::Init()
    {
        auto skyboxShader = ShaderLibrary::GetInstance().Get("Skybox");
        m_SkyboxMaterial = MaterialInstance::Create(Material::Create(skyboxShader));
        m_SkyboxMaterial->SetFlag(MaterialFlag::DepthTest, false);
    }
    
    Entity Scene::CreateEntity(const std::string& name)
    {
        Entity entity{m_Registry.create(),this};

        auto& idComponent = entity.AddComponent<IDComponent>();
        idComponent.ID = {};

        entity.AddComponent<TransformComponent>();
        auto& tag = entity.AddComponent<TagComponent>();
        tag.Tag = name.empty() ? "Entity" : name;
        m_EntityIDMap[idComponent.ID] = entity;
        return entity;
    }
    Entity Scene::CreateEntityWithID(UUID uuid, const std::string& name, bool runtimeMap)
    {
        Entity entity{ m_Registry.create(),this };

        auto& idComponent = entity.AddComponent<IDComponent>();
        idComponent.ID = uuid;

        entity.AddComponent<TransformComponent>();
        auto& tag = entity.AddComponent<TagComponent>();
        tag.Tag = name.empty() ? "Entity" : name;

        PP_CORE_ASSERT(m_EntityIDMap.find(uuid) == m_EntityIDMap.end(), "EntityIDMap already have this entity");
        m_EntityIDMap[uuid] = entity;
        return entity;

    }
    void Scene::DestroyEntity(Entity entity)
    {
        m_Registry.destroy((entt::entity)entity);
    }
    void Scene::OnViewportResize(uint32_t width, uint32_t height)
    {


        m_ViewportWidth = width;
        m_ViewportHeight = height;

        // Resize our non-FixedAspectRatio cameras
        auto view = m_Registry.view<CameraComponent>();
        for (auto entity : view)
        {
            auto& cameraComponent = view.get<CameraComponent>(entity);
            if (!cameraComponent.FixedAspectRatio)
                cameraComponent.Camera.SetViewportSize(width, height);
        }
    }

    void Scene::SetSkybox(const Ref<TextureCube>& skybox)
    {
        m_SkyboxTexture = skybox;
        m_SkyboxMaterial->Set("u_Texture", skybox);
    }

    Entity Scene::GetPrimaryCameraEntity()
    {
        auto view = m_Registry.view<CameraComponent>();
        for (auto entity : view)
        {
            const auto& camera = view.get<CameraComponent>(entity);
            if (camera.Primary)
                return Entity{ entity, this };
        }
        return {};
    }
    void Scene::OnRenderEditor(TimeStep ts, const EditorCamera& editorCamera)
    {
        /////////////////////////////////////////////////////////////////////
        // RENDER 3D SCENE
        /////////////////////////////////////////////////////////////////////

        // Process lights
        {
            m_LightEnvironment = LightEnvironment();
            auto lights = m_Registry.group<DirectionalLightComponent>(entt::get<TransformComponent>);
            uint32_t directionalLightIndex = 0;
            for (auto entity : lights)
            {
                auto [transformComponent, lightComponent] = lights.get<TransformComponent, DirectionalLightComponent>(entity);
                glm::vec3 direction = -glm::normalize(glm::mat3(transformComponent.GetTransform()) * glm::vec3(1.0f));
                m_LightEnvironment.DirectionalLights[directionalLightIndex++] =
                {
                    direction,
                    lightComponent.Radiance,
                    lightComponent.Intensity,
                    lightComponent.CastShadows
                };
            }
        }

        // TODO: only one sky light at the moment!
        {
            m_Environment = Environment();
            auto lights = m_Registry.group<SkyLightComponent>(entt::get<TransformComponent>);
            for (auto entity : lights)
            {
                auto [transformComponent, skyLightComponent] = lights.get<TransformComponent, SkyLightComponent>(entity);
                m_Environment = skyLightComponent.SceneEnvironment;
                m_EnvironmentIntensity = skyLightComponent.Intensity;
                SetSkybox(m_Environment.RadianceMap);
            }
        }

        m_SkyboxMaterial->Set("u_TextureLod", m_SkyboxLod);

        auto group = m_Registry.group<MeshComponent>(entt::get<TransformComponent>);
        SceneRenderer::BeginScene(this, { editorCamera, editorCamera.GetViewMatrix(),
            editorCamera.GetNear(),editorCamera.GetFar(),editorCamera.GetFOV() }); // TODO: real values
        for (auto entity : group)
        {
            auto& [meshComponent, transformComponent] = group.get<MeshComponent, TransformComponent>(entity);
            if (meshComponent.Mesh)
            {
                meshComponent.Mesh->OnUpdate(ts);

                if (m_SelectedEntity == entity)
                    SceneRenderer::SubmitSelectedMesh(meshComponent.Mesh, transformComponent.GetTransform());
                else
                    SceneRenderer::SubmitMesh(meshComponent.Mesh, transformComponent.GetTransform());
            }
        }

        SceneRenderer::EndScene();
        /////////////////////////////////////////////////////////////////////
    }


    void Scene::OnRenderRuntime(TimeStep ts)
    {
        
        //Update Script
        //TODO:Script Engine
        {
            m_Registry.view<NativeScriptComponent>().each([this, &ts](auto entity, auto& nsc)
                {
                    // TODO: Move to Scene::OnScenePlay
                    if (!nsc.Instance)
                    {
                        nsc.Instance = nsc.InstantiateScript();//实例化脚本
                        nsc.Instance->m_Entity = Entity{ entity, this };
                        nsc.Instance->OnCreate();//调用脚本
                    }

                    nsc.Instance->OnUpdate(ts);//调用脚本
                });
        }
        /////////////////////////////////////////////////////////////////////
        // RENDER 3D SCENE
        /////////////////////////////////////////////////////////////////////
        Entity cameraEntity = GetPrimaryCameraEntity();
        if (!cameraEntity)
            return;

        // Process camera entity
        glm::mat4 cameraViewMatrix = glm::inverse(cameraEntity.GetComponent<TransformComponent>().GetTransform());
        PP_CORE_ASSERT(cameraEntity, "Scene does not contain any cameras!");
        SceneCamera& camera = cameraEntity.GetComponent<CameraComponent>();
        camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);

        // Process lights
        {
            m_LightEnvironment = LightEnvironment();
            auto lights = m_Registry.group<DirectionalLightComponent>(entt::get<TransformComponent>);
            uint32_t directionalLightIndex = 0;
            for (auto entity : lights)
            {
                auto [transformComponent, lightComponent] = lights.get<TransformComponent, DirectionalLightComponent>(entity);
                glm::vec3 direction = -glm::normalize(glm::mat3(transformComponent.GetTransform()) * glm::vec3(1.0f));
                m_LightEnvironment.DirectionalLights[directionalLightIndex++] =
                {
                    direction,
                    lightComponent.Radiance,
                    lightComponent.Intensity,
                    lightComponent.CastShadows
                };
            }
        }

        // TODO: only one sky light at the moment!
        {
            m_Environment = Environment();
            auto lights = m_Registry.group<SkyLightComponent>(entt::get<TransformComponent>);
            for (auto entity : lights)
            {
                auto [transformComponent, skyLightComponent] = lights.get<TransformComponent, SkyLightComponent>(entity);
                m_Environment = skyLightComponent.SceneEnvironment;
                m_EnvironmentIntensity = skyLightComponent.Intensity;
                SetSkybox(m_Environment.RadianceMap);
            }
        }

        m_SkyboxMaterial->Set("u_TextureLod", m_SkyboxLod);

        auto group = m_Registry.group<MeshComponent>(entt::get<TransformComponent>);
        SceneRenderer::BeginScene(this, { camera, cameraViewMatrix });
        for (auto entity : group)
        {
            auto [transformComponent, meshComponent] = group.get<TransformComponent, MeshComponent>(entity);
            if (meshComponent.Mesh)
            {
                meshComponent.Mesh->OnUpdate(ts);

                // TODO: Should we render (logically)
                SceneRenderer::SubmitMesh(meshComponent.Mesh, transformComponent.GetTransform(), nullptr);
            }
        }
        SceneRenderer::EndScene();
        /////////////////////////////////////////////////////////////////////
    }


    template<typename T>
    void Scene::OnComponentAdded(Entity entity, T& component)
    {
        static_assert(false);
    }
    template<>
    void Scene::OnComponentAdded<IDComponent>(Entity entity, IDComponent& component)
    {
    }
    template<>
    void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component)
    {
    }

    template<>
    void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
    {
        if (m_ViewportWidth > 0 && m_ViewportHeight > 0)
            component.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
    }

    template<>
    void Scene::OnComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component)
    {
    }

    template<>
    void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component)
    {
    }

    template<>
    void Scene::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component)
    {
    }

    template<>
    void Scene::OnComponentAdded<MeshComponent>(Entity entity, MeshComponent& component)
    {
    }

    template<>
    void Scene::OnComponentAdded<DirectionalLightComponent>(Entity entity, DirectionalLightComponent& component)
    {
    }

    template<>
    void Scene::OnComponentAdded<SkyLightComponent>(Entity entity, SkyLightComponent& component)
    {
    }
}