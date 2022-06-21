#pragma once
#include <Puppet.h>
namespace Puppet {
	class CameraController : public ScriptableEntity
	{
	public:
		virtual void OnCreate() override
		{
			//auto& translation = GetComponent<TransformComponent>().Transform;
			//translation.x = rand() % 10 - 5.0f;
		}

		virtual void OnDestroy() override
		{
		}

		virtual void OnUpdate(TimeStep ts) override
		{

			float speed = 5.0f;
			bool modify = false;
			glm::vec3 translation(0.0f);
			if (Input::IsKeyPressed(Key::A))
				translation.x -= speed * ts, modify = true;
			if (Input::IsKeyPressed(Key::D))
				translation.x += speed * ts, modify = true;
			if (Input::IsKeyPressed(Key::W))
				translation.y += speed * ts, modify = true;
			if (Input::IsKeyPressed(Key::S))
				translation.y -= speed * ts, modify = true;
			
			if (modify)
			{
				PP_CORE_INFO("{0},{1},{2}", translation.x, translation.y, translation.z);
				GetComponent<TransformComponent>().Translation += translation;
			}
		}
	};
}