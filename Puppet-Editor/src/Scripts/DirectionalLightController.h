#pragma once
#include <Puppet.h>
namespace Puppet {
	class DirectionalLightController : public ScriptableEntity
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
			float speed = 3.14/100.0f;
			glm::quat quat = glm::quat(GetComponent<TransformComponent>().Rotation);
			quat=glm::rotate(quat, speed, glm::vec3{ 0,1,0 });
			GetComponent<TransformComponent>().Rotation = glm::eulerAngles(quat);
		}
	};
}