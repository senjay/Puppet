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
                
    }

    Entity Scene::CreateEntity(const std::string& name)
    {
        Entity entity{m_Registry.create(),this};
        entity.AddComponent<TransformComponent>();
        auto& tag = entity.AddComponent<TagComponent>();
        tag.Tag = name.empty() ? "Entity" : name;
        return entity;
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
        SceneRenderer::BeginScene(this, {editorCamera, editorCamera.GetViewMatrix() });
        auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
        for (auto entity : group)
        {
            auto [transformComponent, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
            SceneRenderer::SubmitMesh(transformComponent.Transform, sprite.Color, (int)entity);
        }
        SceneRenderer::EndScene();
    }


    void Scene::OnRenderRuntime(TimeStep ts)
    {
        Entity mainCameraEntity = GetPrimaryCameraEntity();
        if (!mainCameraEntity)
        {
            PP_CORE_ASSERT(mainCameraEntity, "Scene does not contain any cameras!");
            return;
        }
        SceneCamera& mainCamera = mainCameraEntity.GetComponent<CameraComponent>();
        glm::mat4 cameraTransform = mainCameraEntity.GetComponent<TransformComponent>().Transform;
        // Update scripts
        {
            m_Registry.view<NativeScriptComponent>().each([this,&ts](auto entity, auto& nsc)
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
        
        SceneRenderer::BeginScene(this, { mainCamera,  glm::inverse(cameraTransform) });
        auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
        for (auto entity : group)
        {
            auto [transformComponent, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
            SceneRenderer::SubmitMesh(transformComponent.Transform, sprite.Color, (int)entity);
        }
        SceneRenderer::EndScene();
    }

    void Scene::DestroyEntity(Entity entity)
    {
        m_Registry.destroy((entt::entity)entity);
    }


    template<typename T>
    void Scene::OnComponentAdded(Entity entity, T& component)
    {
        static_assert(false);
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
}