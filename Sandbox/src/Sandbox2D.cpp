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


	Renderer2D::ResetStats();
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
		Renderer2D::DrawRotatedQuad({ 1.0f, 0.0f }, { 0.8f, 0.8f }, -45.0f, { 0.8f, 0.2f, 0.3f, 1.0f });
		Renderer2D::DrawQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, { 0.8f, 0.2f, 0.3f, 1.0f });
		Renderer2D::DrawQuad({ 0.5f, -0.5f }, { 0.5f, 0.75f }, m_SquareColor);
		Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.1f }, { 20.0f, 20.0f }, m_Texture, 10.0f);
		Renderer2D::DrawRotatedQuad({ -2.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }, 45, m_Texture, 20.0f);
		Renderer2D::EndScene();
		Renderer2D::BeginScene(m_CameraController->GetCamera());
		for (float y = -5.0f; y < 5.0f; y += 0.5f)
		{
			for (float x = -5.0f; x < 5.0f; x += 0.5f)
			{
				glm::vec4 color = { (x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.7f };
				Renderer2D::DrawQuad({ x, y }, { 0.45f, 0.45f }, color);
			}
		}
		Renderer2D::EndScene();
	}
}

void Sandbox2D::OnUIRender()
{
	PP_PROFILE_FUNCTION();

	ImGui::Begin("Example Layer");
	ImGui::Text("Puppet in Example Layer\n");
	ImGui::Text("FPS: %d\n", m_FPS);
	auto stats=Renderer2D::GetStats();
	ImGui::Text("Renderer2D Stats:");
	ImGui::Text("Draw Calls: %d",stats.DrawCalls);
	ImGui::Text("Quads: %d", stats.QuadCount);
	ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
	ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
	ImGui::ColorEdit4("Quad Color", glm::value_ptr(m_SquareColor));
	ImGui::End();
}

void Sandbox2D::OnEvent(Event& event)
{
	PP_PROFILE_FUNCTION();

	m_CameraController->OnEvent(event);
}
