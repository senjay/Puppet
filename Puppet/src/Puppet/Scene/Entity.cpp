#include "PPpch.h"
#include "Entity.h"
namespace Puppet {

	Entity::Entity(entt::entity handle, Scene* scene)
		:m_EntityHandle(handle), m_Scene(scene)
	{

	}

}
