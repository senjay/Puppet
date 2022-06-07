#include "PPpch.h"
#include "Scene.h"
#include "Entity.h"
#include "Components.h"
#include "Puppet/Renderer/Renderer.h"
#include "Puppet/Renderer/Renderer2D.h"
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

    void Scene::OnUpdate(TimeStep ts)
    {
        // Update scripts
        {
            m_Registry.view<NativeScriptComponent>().each([this,&ts](auto entity, auto& nsc)
                {
                    // TODO: Move to Scene::OnScenePlay
                    if (!nsc.Instance)
                    {
                        nsc.Instance = nsc.InstantiateScript();//ʵ�����ű�
                        nsc.Instance->m_Entity = Entity{ entity, this };
                        nsc.Instance->OnCreate();//���ýű�
                    }

                    nsc.Instance->OnUpdate(ts);//���ýű�
                });
        }

        Camera* mainCamera = nullptr;
        glm::mat4 cameraTransform;
        {
            auto view = m_Registry.view<TransformComponent, CameraComponent>();
            for (auto entity : view)
            {
                auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

                if (camera.Primary)
                {
                    mainCamera = &camera.Camera;
                    cameraTransform = transform.GetTransform();
                    break;
                }
            }
        }

        if (mainCamera)
        {
            Renderer2D::BeginScene(*mainCamera, cameraTransform);

            auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
            for (auto entity : group)
            {
                auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
                Renderer2D::DrawQuad(transform.GetTransform(), sprite.Color);
            }

            Renderer2D::EndScene();
        }
    }

    void Scene::DestroyEntity(Entity entity)
    {
        m_Registry.destroy((entt::entity)entity);
    }

}