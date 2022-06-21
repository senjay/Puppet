#pragma once
#include <Puppet.h>
namespace Puppet {
	class SpriteController : public ScriptableEntity
	{
	public:
		virtual void OnCreate() override
		{

		}

		virtual void OnDestroy() override
		{
		}

		virtual void OnUpdate(TimeStep ts) override
		{
			float speed = 0.1f;
			GetComponent<TransformComponent>().Translation += glm::vec3(0, 0, ts * speed);		
		}
	};
}