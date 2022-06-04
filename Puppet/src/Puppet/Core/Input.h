#pragma once
#include <glm/glm.hpp>
#include "Puppet/Core/KeyCodes.h"
#include "Puppet/Core/MouseCodes.h"
namespace Puppet
{
	class Input
	{
	public:
		static bool IsKeyPressed(KeyCode key);
		static bool IsMouseButtonPressed(MouseCode button);
		static glm::vec2 GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();
	};
}
