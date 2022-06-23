#include "EditorLayer.h"
#include <ImGuizmo/ImGuizmo.h>
#include "Puppet/Renderer/SceneRenderer.h"
#include "Scripts/CameraController.h"
#include "Scripts/SpriteController.h"
#include "Scripts/DirectionalLightController.h"
#include "ImGuiUtils/ImGuiWrapper.h"
#include <glm/glm.hpp>
namespace Puppet {
	extern const std::filesystem::path g_AssetPath;

	EditorLayer::EditorLayer() : Layer("EditorLayer")
	{

	}

	void EditorLayer::OnAttach()
	{
		PP_PROFILE_FUNCTION();

		m_IconPlay = Texture2D::Create("./Resources/Icons/PlayButton.png");
		m_IconStop = Texture2D::Create("./Resources/Icons/StopButton.png");

		m_EditorCamera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);
		m_ActiveScene = CreateRef<Scene>();
		m_ActiveScene->Init();
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);	
	}

	void EditorLayer::OnDetach()
	{
		PP_PROFILE_FUNCTION();

	}

	void EditorLayer::OnUpdate(TimeStep ts)
	{
		PP_PROFILE_FUNCTION();

		Renderer2D::ResetStats();
		float tsSec = ts.GetSeconds();
		m_FPS = static_cast<int>(1.0 / tsSec);


		switch (m_SceneState)
		{
			case SceneState::Edit:
			{
				m_EditorCamera.OnUpdate(ts);

				m_ActiveScene->OnRenderEditor(ts, m_EditorCamera);
				break;
			}
			case SceneState::Play:
			{	
				m_ActiveScene->OnRenderRuntime(ts);
				break;
			}
		}

		{
			PP_PROFILE_SCOPE("Mouse Pick Read Data");
			auto [mx, my] = ImGui::GetMousePos();
			mx -= m_ViewportBounds[0].x;
			my -= m_ViewportBounds[0].y;
			glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
			my = viewportSize.y - my;
			int mouseX = (int)mx;
			int mouseY = (int)my;
			
			if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y)
			{
				//Renderer::WaitAndRender();
				//int pixelData=SceneRenderer::GetFinalRenderPass()->GetSpecification().TargetFramebuffer->ReadPixel(1, mouseX, mouseY);
				//m_HoveredEntity = pixelData ==-1 ? Entity() : Entity((entt::entity)pixelData, m_ActiveScene.get());
				//PP_CORE_INFO("mouse coord:{0},{1}, mouse data:{2}", mouseX, mouseY, pixelData);
			}
		}
	}

	void EditorLayer::OnUIRender()
	{
		PP_PROFILE_FUNCTION();
		static bool dockspaceOpen = true;
		static bool opt_fullscreen_persistant = true;
		bool opt_fullscreen = opt_fullscreen_persistant;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive, 
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise 
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// DockSpace
		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		float minWinSizeX = style.WindowMinSize.x;
		style.WindowMinSize.x = 370.0f;
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}
		style.WindowMinSize.x = minWinSizeX;
		
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				// Disabling fullscreen would allow the window to be moved to the front of other windows, 
				// which we can't undo at the moment without finer window depth/z control.
				//ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);
				if (ImGui::MenuItem("New Scene", "Ctrl+N"))
					NewScene();
				if (ImGui::MenuItem("Open Scene", "Ctrl+O"))
					OpenScene();
				ImGui::Separator();
				if (ImGui::MenuItem("Save Scene", "Ctrl+S"))
					SaveScene();
				if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S"))
					SaveSceneAs();
				ImGui::Separator();
				if (ImGui::MenuItem("Exit")) Application::Get().Close();
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Window"))
			{
				ImGui::MenuItem("Content Browser", NULL, &b_ShowContentBrowser);
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
		static bool show = true;
		ImGui::ShowDemoWindow(&show);

		m_SceneHierarchyPanel.OnImGuiRender();
		if(b_ShowContentBrowser)
			m_ContentBrowserPanel.OnImGuiRender(&b_ShowContentBrowser);


		ImGui::Begin("Environment");
		ImGui::SliderFloat("Skybox LOD", &m_ActiveScene->GetSkyboxLod(), 0.0f, 11.0f);
		ImGui::AlignTextToFramePadding();
		ImGuiUtils::BeginPropertyGrid();
		auto& light = m_ActiveScene->GetLight();
		ImGuiUtils::Property("Light Direction", light.Direction);
		ImGuiUtils::Property("Light Radiance", light.Radiance);
		ImGuiUtils::Property("Light Multiplier", light.Multiplier, 0.05,0.0f, 5.0f);
		ImGuiUtils::Property("Exposure", m_EditorCamera.GetExposure(), 0.05, 0.0f, 5.0f);
		ImGuiUtils::Property("Radiance Prefiltering", m_RadiancePrefilter);
		ImGuiUtils::Property("Env Map Rotation", m_EnvMapRotation, 1, -360.0f, 360.0f);
		ImGuiUtils::EndPropertyGrid();
		ImGui::End();


		ImGui::Begin("Puppet Editor");
		ImGui::Text("Puppet in Example Layer\n");
		ImGui::Text("FPS: %d\n", m_FPS);
		ImGui::Text("Frame Time: %.2fms\n", 1000.0/ m_FPS);
		auto [rcCnt, rcSize] = Renderer::GetCommandQueueStatus();
		ImGui::Text("RenderCommand Cnt: %d\n", rcCnt);
		ImGui::Text("RenderCommand Size: %d Bytes\n", rcSize);
		auto stats = Renderer2D::GetStats();
		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls: %d", stats.DrawCalls);
		ImGui::Text("Quads: %d", stats.QuadCount);
		ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
		ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
		std::string name = "None";
		if (m_HoveredEntity)
			name = m_HoveredEntity.GetComponent<TagComponent>().Tag;
		ImGui::Text("Hovered Entity: %s", name.c_str());
		auto& caps = RenderCommand::GetCapabilities();
		ImGui::Text("Vendor: %s", caps.Vendor.c_str());
		ImGui::Text("Renderer: %s", caps.Renderer.c_str());
		ImGui::Text("Version: %s", caps.Version.c_str());
		ImGui::End();

		UI_Toolbar();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("ViewPort");

		auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
		auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		auto viewportOffset = ImGui::GetWindowPos();
		m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
		m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };
		
		ImGui::GetForegroundDrawList()->AddRect(*(ImVec2*)&m_ViewportBounds[0], *(ImVec2*)&m_ViewportBounds[1], IM_COL32(255, 255, 0, 255));
		
		m_ViewportFocused = ImGui::IsWindowFocused();
		m_ViewportHovered = ImGui::IsWindowHovered();
		Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused && !m_ViewportHovered);
		
		ImVec2 viewportPanelSize=ImGui::GetContentRegionAvail();
		m_ViewportSize = { viewportPanelSize.x,viewportPanelSize.y };


		//TODO: move to other?
		SceneRenderer::SetViewportSize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
		m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);

		
		//show viewport
		uint32_t textureid = SceneRenderer::GetFinalColorBufferRendererID();
		ImGui::Image((void*)textureid, ImVec2(m_ViewportSize.x, m_ViewportSize.y), ImVec2(0, 1), ImVec2(1, 0));

		//DragSceneFile
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				const wchar_t* path = (const wchar_t*)payload->Data;
				OpenScene(std::filesystem::path(g_AssetPath) / path);
			}
			ImGui::EndDragDropTarget();
		}


		//Gizmo
		Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
		if (selectedEntity && m_GizmoType != -1)
		{
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();
			ImGuizmo::SetRect(m_ViewportBounds[0].x, m_ViewportBounds[0].y, m_ViewportBounds[1].x - m_ViewportBounds[0].x, m_ViewportBounds[1].y - m_ViewportBounds[0].y);
			
			// Editor camera
			const glm::mat4& cameraProjection = m_EditorCamera.GetProjection();
			glm::mat4 cameraView = m_EditorCamera.GetViewMatrix();

			// Entity transform
			auto& tc = selectedEntity.GetComponent<TransformComponent>();
			glm::mat4 transform = tc.GetTransform();

			// Snapping
			bool snap = Input::IsKeyPressed(Key::LeftControl);
			float snapValue = 0.5f; // Snap to 0.5m for translation/scale
			// Snap to 45 degrees for rotation
			if (m_GizmoType == ImGuizmo::OPERATION::ROTATE)
				snapValue = 45.0f;

			float snapValues[3] = { snapValue, snapValue, snapValue };

			ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
				(ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform),
				nullptr, snap ? snapValues : nullptr);

			if (ImGuizmo::IsUsing())
			{
				glm::vec3 translation, rotation, scale;
				Math::DecomposeTransform(transform, translation, rotation, scale);

				glm::vec3 deltaRotation = rotation - tc.Rotation;
				tc.Translation = translation;
				tc.Rotation += deltaRotation;
				tc.Scale = scale;
			}
		}
		


		ImGui::End();
		ImGui::PopStyleVar();
		
		ImGui::End();	
	}

	void EditorLayer::OnEvent(Event& event)
	{
		PP_PROFILE_FUNCTION();

		m_EditorCamera.OnEvent(event);

		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<KeyPressedEvent>(PP_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
		dispatcher.Dispatch<MouseButtonPressedEvent>(PP_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
	}

	bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
	{
		if (e.GetRepeatCount() > 0)
			return false;
		bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
		bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);

		switch (e.GetKeyCode())
		{
			case Key::N:
			{
				if (control)
					NewScene();
				break;
			}
			case Key::O:
			{
				if (control)
					OpenScene();
				break;
			}
			case Key::S:
			{
				if (control && shift)
					SaveSceneAs();
				else if (control)
					SaveScene();
				break;
			}
			case Key::Space:
			{
				OpenScene("assets/scenes/test.scene");
				break;
			}
			case Key::Q:
				if (!ImGuizmo::IsUsing())
					m_GizmoType = -1;
				break;
			case Key::W:
				if (!ImGuizmo::IsUsing())
					m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
				break;
			case Key::E:
				if (!ImGuizmo::IsUsing())
					m_GizmoType = ImGuizmo::OPERATION::ROTATE;
				break;
			case Key::R:
				if (!ImGuizmo::IsUsing())
					m_GizmoType = ImGuizmo::OPERATION::SCALE;
				break;
		}
	}

	bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		if (e.GetMouseButton() == Mouse::ButtonLeft)
		{
			//bool check: can use mouse select Entity
			if (m_ViewportHovered)
			{
				auto [mouseX, mouseY] = GetMouseViewportSpace();
				if (mouseX > -1.0f && mouseX < 1.0f && mouseY > -1.0f && mouseY < 1.0f)
				{
					auto [origin, direction] = CastRay(mouseX, mouseY);

					m_HoveredEntity = Entity();
					m_SceneHierarchyPanel.SetSelectedEntity(m_HoveredEntity);
					auto meshEntities = m_ActiveScene->GetAllEntitiesWith<MeshComponent>();
					for (auto e : meshEntities)
					{
						Entity entity = { e, m_ActiveScene.get() };
						auto mesh = entity.GetComponent<MeshComponent>().Mesh;
						if (!mesh)
							continue;

						auto& submeshes = mesh->GetSubmeshes();
						float lastT = std::numeric_limits<float>::max();
						for (uint32_t i = 0; i < submeshes.size(); i++)
						{
							Submesh& submesh = submeshes[i];
							glm::mat4 ts = entity.GetComponent<TransformComponent>().GetTransform();
							Ray ray = {
								glm::inverse(ts * submesh.Transform) * glm::vec4(origin, 1.0f),
								glm::inverse(glm::mat3(ts) * glm::mat3(submesh.Transform)) * direction
							};

							float t;
							bool intersects = ray.IntersectsAABB(submesh.BoundingBox, t);
							if (intersects)
							{
								const auto& triangleCache = mesh->GetTriangleCache(i);
								for (const auto& triangle : triangleCache)
								{
									if (ray.IntersectsTriangle(triangle.V0.Position, triangle.V1.Position, triangle.V2.Position, t))
									{
										PP_WARN("INTERSECTION: {0}, t={1}", submesh.NodeName, t);
										if (t < lastT)
										{
											lastT = t;
											m_HoveredEntity = entity;

										}
										//m_SelectionContext.push_back({ entity, &submesh, t });
										break;
									}
								}
							}
						}
					}
					//std::sort(m_SelectionContext.begin(), m_SelectionContext.end(), [](auto& a, auto& b) { return a.Distance < b.Distance; });
					//if (m_SelectionContext.size())
						//OnSelected(m_SelectionContext[0]);
				}
			}
			if (m_ViewportHovered && !ImGuizmo::IsOver() && !Input::IsKeyPressed(Key::LeftAlt) && m_SceneState != SceneState::Play)
				m_SceneHierarchyPanel.SetSelectedEntity(m_HoveredEntity);
			
		}
		return false;
	}

	void EditorLayer::OnScenePlay()
	{
		m_SceneState = SceneState::Play;
	}

	void EditorLayer::OnSceneStop()
	{
		m_SceneState = SceneState::Edit;
	}

	void EditorLayer::NewScene()
	{
		m_ActiveScene = CreateRef<Scene>();
		m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
		m_SceneFilePath = std::string();
	}

	void EditorLayer::OpenScene()
	{
		std::string filepath = FileDialogs::OpenFile("Puppet Scene (*.scene)\0*.scene\0");
		if (!filepath.empty())
			OpenScene(filepath);
	}

	void EditorLayer::OpenScene(const std::filesystem::path& path)
	{
		if (path.extension().string() != ".scene")
		{
			PP_CORE_WARN("Could not load {0} - not a scene file", path.filename().string());
			return;
		}

		Ref<Scene> newScene = CreateRef<Scene>();
		SceneSerializer serializer(newScene);
		m_SceneFilePath = path.string();
		if (serializer.Deserialize(m_SceneFilePath))
		{
			m_ActiveScene = newScene;
			m_ActiveScene->Init();
			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_SceneHierarchyPanel.SetContext(m_ActiveScene);
			if(m_ActiveScene->GetSceneName()=="TestScene")
				CreateBindTestScripts();
		}
	}

	void EditorLayer::SaveScene()
	{
		if (!m_SceneFilePath.empty())
		{
			SceneSerializer serializer(m_ActiveScene);
			serializer.Serialize(m_SceneFilePath);
			PP_CORE_INFO("Save scene to {0}", m_SceneFilePath);
		}
		else
		{
			SaveSceneAs();
		}
	}

	void EditorLayer::SaveSceneAs()
	{
		
		std::string filepath = FileDialogs::SaveFile("Hazel Scene (*.scene)\0*.scene\0");
		if (!filepath.empty())
		{
			SceneSerializer serializer(m_ActiveScene);
			serializer.Serialize(filepath);
			m_SceneFilePath = filepath;
			PP_CORE_INFO("Save scene to {0}", m_SceneFilePath);
		}
		else
			PP_CORE_WARN("Save scene to {0} failed!", m_SceneFilePath);
	}

	void EditorLayer::UI_Toolbar()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		auto& colors = ImGui::GetStyle().Colors;
		const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
		const auto& buttonActive = colors[ImGuiCol_ButtonActive];
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));

		ImGui::Begin("##toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		float size = ImGui::GetWindowHeight() - 4.0f;
		Ref<Texture2D> icon = m_SceneState == SceneState::Edit ? m_IconPlay : m_IconStop;
		ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
		if (ImGui::ImageButton((ImTextureID)icon->GetRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0))
		{
			if (m_SceneState == SceneState::Edit)
				OnScenePlay();
			else if (m_SceneState == SceneState::Play)
				OnSceneStop();
		}
		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(3);
		ImGui::End();
	}

	void EditorLayer::CreateBindTestScripts()
	{
		Entity PrimaryCameraEntity =m_ActiveScene->GetPrimaryCameraEntity();
		PrimaryCameraEntity.AddComponent<NativeScriptComponent>().Bind<CameraController>();
		auto view= m_ActiveScene->GetAllEntitiesWith<DirectionalLightComponent>();
		for (auto entity : view)
		{
			Entity dirlightEntity= Entity{ entity, m_ActiveScene.get() };
			dirlightEntity.AddComponent<NativeScriptComponent>().Bind<DirectionalLightController>();
		}
	}

	std::pair<float, float> EditorLayer::GetMouseViewportSpace()
	{
		auto [mx, my] = ImGui::GetMousePos();
		mx -= m_ViewportBounds[0].x;
		my -= m_ViewportBounds[0].y;
		glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
		my = viewportSize.y - my;
		return { (mx / viewportSize.x) * 2.0f - 1.0f, ((my / viewportSize.y) * 2.0f - 1.0f) };
	}

	std::pair<glm::vec3, glm::vec3> EditorLayer::CastRay(float mx, float my)
	{
		glm::vec4 mouseClipPos = { mx, my, -1.0f, 1.0f };

		auto inverseProj = glm::inverse(m_EditorCamera.GetProjection());
		auto inverseView = glm::inverse(glm::mat3(m_EditorCamera.GetViewMatrix()));

		glm::vec4 ray = inverseProj * mouseClipPos;
		glm::vec3 rayPos = m_EditorCamera.GetPosition();
		glm::vec3 rayDir = inverseView * glm::vec3(ray);

		return { rayPos, rayDir };
	}


}