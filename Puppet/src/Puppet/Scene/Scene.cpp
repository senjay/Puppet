#include "PPpch.h"
#include "Scene.h"
#include <glm/glm.hpp>
namespace Puppet {

    Scene::Scene()
    {
        m_Registry.create();
    }

    Scene::~Scene()
    {
    }

    entt::entity Scene::CreateEntity(const std::string& name)
    {
        return m_Registry.create();
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