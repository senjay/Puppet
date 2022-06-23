#pragma once
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Puppet.h>
#include <imgui/imgui.h>
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/ContentBrowserPanel.h"
#include "Puppet/Camera/EditorCamera.h"
namespace Puppet
{
	class EditorLayer :public Layer
	{
	public:
		EditorLayer();
		~EditorLayer() = default;
		virtual void OnAttach()override;

		virtual void OnDetach()override;

		virtual void OnUpdate(TimeStep ts)override;

		virtual void OnUIRender() override;

		virtual void OnEvent(Event& event) override;
	private:
		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);

		void OnScenePlay();
		void OnSceneStop();

		void NewScene();
		void OpenScene();
		void OpenScene(const std::filesystem::path& path);
		void SaveScene();
		void SaveSceneAs();

		void UI_Toolbar();
		void CreateBindTestScripts();
		std::pair<float, float> GetMouseViewportSpace();
		std::pair<glm::vec3, glm::vec3> CastRay(float mx,float my);
	private:
		Ref<Scene>m_ActiveScene;
		EditorCamera m_EditorCamera;
		Entity m_HoveredEntity;
		/*struct SelectedSubmesh
		{
			Puppet::Entity Entity;
			Submesh* Mesh = nullptr;
			float Distance = 0.0f;
		};
		std::vector<SelectedSubmesh> m_SelectionContext;*/

		SceneHierarchyPanel m_SceneHierarchyPanel;
		ContentBrowserPanel m_ContentBrowserPanel;
		
		std::string m_SceneFilePath;
		
		int m_GizmoType = -1;

		enum class SceneState
		{
			Edit = 0, Play = 1
		};
		SceneState m_SceneState = SceneState::Edit;

		bool m_ViewportFocused = false, m_ViewportHovered = false;
		glm::vec2 m_ViewportSize{ 0,0 };
		glm::vec2 m_ViewportBounds[2];
		int m_FPS;

		Ref<Texture2D> m_IconPlay, m_IconStop;
		// PBR params
		bool m_RadiancePrefilter = false;
		float m_EnvMapRotation = 0.0f;

		//window
		bool b_ShowContentBrowser = true;
	};
}


