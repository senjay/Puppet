#include "CameraController.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Puppet {


	float CameraController::RotationSpeed() const
	{
		return 0.8f;
	}
	void CameraController::OnUpdate(TimeStep ts)
	{
		
		auto& cameraRotation = GetComponent<TransformComponent>().Rotation;
		auto& cameraTranslation = GetComponent<TransformComponent>().Translation;
		
		const glm::vec2& mouse{ Input::GetMouseX(), Input::GetMouseY() };
		glm::vec2 delta = (mouse - m_InitialMousePosition) * 0.003f;
		m_InitialMousePosition = mouse;

		MouseRotate(delta);
		
		glm::quat orientation = GetOrientation();
		cameraRotation = glm::eulerAngles(orientation);

		if (Input::IsKeyPressed(Key::A))
		{
			cameraTranslation -= GetRightDirection() * (m_MoveSpeed * ts);
		}
		if (Input::IsKeyPressed(Key::D))
		{
			cameraTranslation += GetRightDirection() * (m_MoveSpeed * ts);
		}
		if (Input::IsKeyPressed(Key::W))
		{
			cameraTranslation += GetForwardDirection() *(m_MoveSpeed *ts);
		}
		if (Input::IsKeyPressed(Key::S))
		{
			cameraTranslation -= GetForwardDirection() * (m_MoveSpeed * ts);
		}
		
	}

	void CameraController::MouseRotate(const glm::vec2& delta)
	{
		float yawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;
		m_Yaw += yawSign * delta.x * RotationSpeed();
		m_Pitch += delta.y * RotationSpeed();
	}

	glm::vec3 CameraController::GetUpDirection() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	glm::vec3 CameraController::GetRightDirection() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
	}

	glm::vec3 CameraController::GetForwardDirection() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
	}

	glm::quat CameraController::GetOrientation() const
	{
		return glm::quat(glm::vec3(-m_Pitch, -m_Yaw, 0.0f));
	}
}
