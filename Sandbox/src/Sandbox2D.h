#pragma once
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Puppet.h>
#include "imgui.h"
using namespace Puppet;
class Sandbox2D:public Layer
{
public:
	Sandbox2D();
	~Sandbox2D()=default;
	virtual void OnAttach()override;

	virtual void OnDetach()override;

	virtual void OnUpdate(TimeStep ts)override;

	virtual void OnUIRender() override;

	virtual void OnEvent(Event& event) override;
private:
	Scope<ShaderLibrary>m_ShaderLibrary;
	Ref<OrthographicCameraController>m_CameraController;
	Ref<Texture2D>m_Texture;
	Ref<Framebuffer>m_Framebuffer;
	glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };
	int m_FPS;
};

