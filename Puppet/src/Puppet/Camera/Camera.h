#pragma once
#include <glm/glm.hpp>

namespace Puppet {
	class Camera
	{
	public:
		Camera() = default;
		Camera(const glm::mat4& projection)
			: m_Projection(projection) {}

		virtual ~Camera() = default;

		const glm::mat4& GetProjection() const { return m_Projection; }
		float GetExposure() const { return m_Exposure; }
		float& GetExposure() { return m_Exposure; }
	protected:
		glm::mat4 m_Projection = glm::mat4(1.0f);
		float m_Exposure = 0.8f;
	};
}
