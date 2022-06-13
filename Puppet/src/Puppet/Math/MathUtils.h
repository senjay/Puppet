#pragma once
#include <glm/glm.hpp>

namespace Puppet::Math {

	bool DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale);
	// translation, orientation, scale 
	std::tuple<glm::vec3, glm::quat, glm::vec3>DecomposeTransform(const glm::mat4& transform);
}