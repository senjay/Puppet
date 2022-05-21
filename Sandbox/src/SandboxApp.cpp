#include <Puppet.h>

class ExampleLayer : public Puppet::Layer
{
public:
	ExampleLayer() : Layer("Example")
	{

	}

	void OnUpdate() override
	{
		PP_INFO("ExampleLayer::Update");
	}

	void OnEvent(Puppet::Event& event) override
	{
		PP_TRACE("{0}", event);
	}

};

class Sandbox :public Puppet::Application
{
public:
	Sandbox()
	{
		//PushLayer(new ExampleLayer());
		PushLayer(new Puppet::ImGuiLayer());
	}
	~Sandbox(){}

};
Puppet::Application* Puppet::CreateApplication()
{
	return new Sandbox();
}
