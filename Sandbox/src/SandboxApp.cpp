#include <Puppet.h>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/ext/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale
#include <glm/ext/matrix_clip_space.hpp> // glm::perspective
#include <glm/ext/scalar_constants.hpp> // glm::pi

glm::mat4 camera(float Translate, glm::vec2 const& Rotate)
{
	glm::mat4 Projection = glm::perspective(glm::pi<float>() * 0.25f, 4.0f / 3.0f, 0.1f, 100.f);
	glm::mat4 View = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -Translate));
	View = glm::rotate(View, Rotate.y, glm::vec3(-1.0f, 0.0f, 0.0f));
	View = glm::rotate(View, Rotate.x, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 Model = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
	return Projection * View * Model;
}

class ExampleLayer : public Puppet::Layer
{
public:
	ExampleLayer() : Layer("Example")
	{
		auto cam = camera(0.5f, { 0.3f, 0.4f });
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
	}
	~Sandbox(){}

};
Puppet::Application* Puppet::CreateApplication()
{
	return new Sandbox();
}
