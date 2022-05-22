#include <Puppet.h>

class ExampleLayer : public Puppet::Layer
{
public:
	ExampleLayer() : Layer("Example")
	{

	}

	void OnUpdate() override
	{
		if (Puppet::InputSystem::getInstance().IsKeyPressed(Puppet::Key::Tab))
			PP_INFO("TAB pressed!");
	}

	void OnEvent(Puppet::Event& event) override
	{
		if (event.GetEventType() == Puppet::EventType::KeyPressed)
		{
			Puppet::KeyPressedEvent& e = (Puppet::KeyPressedEvent&)event;
			PP_TRACE("{0}", (char)e.GetKeyCode());
		}
	}

};

class Sandbox :public Puppet::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
		PushLayer(new Puppet::ImGuiLayer());
	}
	~Sandbox(){}

};
Puppet::Application* Puppet::CreateApplication()
{
	return new Sandbox();
}
