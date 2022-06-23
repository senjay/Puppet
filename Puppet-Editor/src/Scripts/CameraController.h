#pragma once
#include <Puppet.h>
namespace Puppet {
	class CameraController :public ScriptableEntity
	{
	public:

		virtual void OnCreate() override {}
		virtual void OnDestroy() override {}
		virtual void OnUpdate(TimeStep ts) override;
		float GetDistance() const { return m_Distance; }
		void SetDistance(float distance) { m_Distance = distance; }
		glm::vec3 GetUpDirection() const;
		glm::vec3 GetRightDirection() const;
		glm::vec3 GetForwardDirection() const;
		glm::quat GetOrientation() const;
		float GetPitch() const { return m_Pitch; }
		float GetYaw() const { return m_Yaw; }

	private:
		void MouseRotate(const glm::vec2& delta);
		float RotationSpeed() const;
	private:
		glm::vec3 m_FocalPoint = { 0.0f, 0.0f, 0.0f };
		glm::vec2 m_InitialMousePosition = { 0.0f, 0.0f };
		float m_Distance = 10.0f;
		float m_Pitch = 0.0f, m_Yaw = 0.0f;
		float m_MoveSpeed = 40.0f;
	};
}
