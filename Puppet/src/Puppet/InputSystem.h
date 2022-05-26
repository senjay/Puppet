#pragma once
#include "Input.h"
#include "CoreSingleton.h"
namespace Puppet
{
	class PUPPET_API InputSystem :public CoreSingleton<InputSystem>
	{
		friend class CoreSingleton<InputSystem>;
	protected:
		InputSystem() = default;
	public:
		void initialize(Input* input) 
		{
			PP_CORE_ASSERT(input, "Input System failed to Initialized!");
			m_input = Ref<Input>(input);
		}
		bool IsKeyPressed(int keyCode) { return m_input->IsKeyPressedImpl(keyCode); }
		bool IsMouseButtonPressed(int button) { return m_input->IsMouseButtonPressedImpl(button); }
		std::pair<float, float> GetMousePosition() { return m_input->GetMousePositionImpl(); }
		float GetMouseX() { return m_input->GetMouseXImpl(); }
		float GetMouseY() { return m_input->GetMouseYImpl(); }
	private:
		Ref<Input>m_input;
	};
}