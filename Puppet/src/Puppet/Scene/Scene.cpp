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
        m_Registry.create();
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

    void Scene::OnUpdate(TimeStep st)
    {
        auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
        for (auto entity : group)
        {
            auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
            Renderer2D::DrawQuad(transform.GetTransform(), sprite.Color);
        }
    }

}