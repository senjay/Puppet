#include "Sandbox2D.h"

Sandbox2D::Sandbox2D() : Layer("Sandbox2D")
{
	m_CameraController = CreateRef<OrthographicCameraController>(1280.0f / 720, true);
	m_ShaderLibrary = CreateScope<ShaderLibrary>();
}

void Sandbox2D::OnAttach()
{
	float vertices[4 * 5] = {
		-0.5, -0.5, 0,
		 0.5, -0.5, 0,
		 0.5,  0.5, 0,
		-0.5,  0.5, 0,
	};
	uint32_t indices[6] = { 0,1,2,2,3,0 };
	Ref<VertexBuffer>QuadVBuffer;
	QuadVBuffer = VertexBuffer::Create(vertices, sizeof(vertices));
	{
		BufferLayout layout = {
		{ShaderDataType::Float3,"a_Position"},
		};
		QuadVBuffer->SetLayout(layout);
	}
	Ref<IndexBuffer>QuadIndexBuff;
	QuadIndexBuff = IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));
	m_SquareVA = VertexArray::Create();
	m_SquareVA->AddVertexBuffer(QuadVBuffer);
	m_SquareVA->SetIndexBuffer(QuadIndexBuff);
	m_FlatColorShader= m_ShaderLibrary->Load("./assets/shaders/FlatColor.glsl");
	//m_Texture = Texture2D::Create("./assets/textures/Checkerboard.png");
}

void Sandbox2D::OnDetach()
{
	
}

void Sandbox2D::OnUpdate(TimeStep ts)
{
	PP_TRACE("Delta time: {0}s ,{1}ms", ts.GetSeconds(), ts.GetMillseconds());
	float tsSec = ts.GetSeconds();
	m_FPS = static_cast<int>(1.0 / tsSec);
	m_CameraController->OnUpdate(ts);

	RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
	RenderCommand::Clear();

	Renderer::BeginScene(m_CameraController->GetCamera());
	m_FlatColorShader->Bind();
	m_FlatColorShader->SetFloat4("u_Color", m_SquareColor);
	Renderer::Submit(m_FlatColorShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));
	Renderer::EndSence();

}

void Sandbox2D::OnUIRender()
{
	ImGui::Begin("Example Layer");
	ImGui::Text("Puppet in Example Layer\n");
	ImGui::Text("FPS: %d\n", m_FPS);
	ImGui::ColorEdit4("Quad Color", glm::value_ptr(m_SquareColor));
	ImGui::End();
}

void Sandbox2D::OnEvent(Event& event)
{
	m_CameraController->OnEvent(event);
}
