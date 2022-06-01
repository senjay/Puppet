#include "Sandbox2D.h"

Sandbox2D::Sandbox2D() : Layer("Sandbox2D")
{
	m_CameraController = CreateRef<OrthographicCameraController>(1280.0f / 720, true);
	m_ShaderLibrary = CreateScope<ShaderLibrary>();
}

void Sandbox2D::OnAttach()
{
	PP_PROFILE_FUNCTION();

	m_Texture = Texture2D::Create("./assets/textures/Checkerboard.png");
}

void Sandbox2D::OnDetach()
{
	PP_PROFILE_FUNCTION();

}

void Sandbox2D::OnUpdate(TimeStep ts)
{
	PP_PROFILE_FUNCTION();

	PP_TRACE("Delta time: {0}s ,{1}ms", ts.GetSeconds(), ts.GetMillseconds());
	float tsSec = ts.GetSeconds();
	m_FPS = static_cast<int>(1.0 / tsSec);
	m_CameraController->OnUpdate(ts);

	{
		PP_PROFILE_SCOPE("Renderer Prep");
		RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		RenderCommand::Clear();
	}
	{
		PP_PROFILE_SCOPE("Renderer Draw");
		Renderer2D::BeginScene(m_CameraController->GetCamera());
		Renderer2D::DrawQuad({ 1.0f,1.0f }, { 2,1.5 }, m_SquareColor);
		Renderer2D::DrawQuad({ -1,0 }, { 1,1 }, { 0.0,0.2,1.0,1 });
		Renderer2D::DrawQuad({ -1.0f, 0.0f }, { 2.0f, 2.8f }, { 0.0f, 0.8f, 0.3f, 1.0f });
		//Renderer2D::DrawQuad({ 0.5f, -0.5f }, { 0.5f, 0.75f }, { 0.2f, 0.3f, 0.8f, 1.0f });
		Renderer2D::DrawQuad({ 0,0,-0.1 }, { 10,10 }, m_Texture, 10.0f);
		//Renderer2D::DrawRotatedQuad({ -1,1 }, { 1,1 }, 30,{ 0.8,0.2,0.3,1 });
		//Renderer2D::DrawRotatedQuad({ 1,1 }, { 1,1 }, 45,m_Texture, 5.0f, { 0.1,0.8,0.2,1.0 });
		Renderer2D::EndScene();
	}
}

void Sandbox2D::OnUIRender()
{
	PP_PROFILE_FUNCTION();

	ImGui::Begin("Example Layer");
	ImGui::Text("Puppet in Example Layer\n");
	ImGui::Text("FPS: %d\n", m_FPS);
	ImGui::ColorEdit4("Quad Color", glm::value_ptr(m_SquareColor));
	ImGui::End();
}

void Sandbox2D::OnEvent(Event& event)
{
	PP_PROFILE_FUNCTION();

	m_CameraController->OnEvent(event);
}
