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

		void NewScene();
		void OpenScene();
		void OpenScene(const std::filesystem::path& path);
		void SaveScene();
		void SaveSceneAs();
	private:
		Scope<ShaderLibrary>m_ShaderLibrary;
		Ref<Texture2D>m_Texture;
		Ref<Framebuffer>m_Framebuffer;
		Ref<Framebuffer>m_DrawFramebuffer;

		Ref<Scene>m_ActiveScene;
		EditorCamera m_EditorCamera;
		Entity m_HoveredEntity;

		SceneHierarchyPanel m_SceneHierarchyPanel;
		ContentBrowserPanel m_ContentBrowserPanel;
		
		std::string m_SceneFilePath;
		
		int m_GizmoType = -1;

		bool m_ViewportFocused = false, m_ViewportHovered = false;
		glm::vec2 m_ViewportSize{ 0,0 };
		glm::vec2 m_ViewportBounds[2];
		int m_FPS;
	};
}


