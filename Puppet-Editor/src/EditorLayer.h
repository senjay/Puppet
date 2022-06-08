#pragma once
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Puppet.h>
#include <imgui/imgui.h>
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/ContentBrowserPanel.h"
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
		Scope<ShaderLibrary>m_ShaderLibrary;
		Ref<OrthographicCameraController>m_CameraController;
		Ref<Texture2D>m_Texture;
		Ref<Framebuffer>m_Framebuffer;

		Ref<Scene>m_ActiveScene;
		Entity m_SquareEntity;
		Entity m_CameraEntity;
		SceneHierarchyPanel m_SceneHierarchyPanel;
		ContentBrowserPanel m_ContentBrowserPanel;
		
		bool m_ViewportFocused = false, m_ViewportHovered = false;
		glm::vec2 m_ViewportSize{ 0,0 };
		int m_FPS;
	};
}


