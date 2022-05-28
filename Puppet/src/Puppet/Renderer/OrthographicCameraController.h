#pragma once
#include "OrthographicCamera.h"
#include "Puppet/Core/Timestep.h"
#include "Puppet/Events/ApplicationEvent.h"
#include "Puppet/Events/MouseEvent.h"
namespace Puppet {
	class OrthographicCameraController
	{
	public:
		// eqution:aspectRatio,zoomLevel->orthoMatrix
		// glm::ortho(-aspectRatio* zoomLevel, aspectRatio* zoomLevel, -zoomLevel, zoomLevel, -1.0f, 1.0f)
		OrthographicCameraController(float aspectRatio, bool rotation=false);
		void OnUpdate(TimeStep ts);
		void OnEvent(Event& e);

		void OnResize(float width, float height);
		OrthographicCamera& GetCamera() { return m_Camera; }
		const OrthographicCamera& GetCamera() const { return m_Camera; }

		float GetZoomLevel() const { return m_ZoomLevel; }
		void SetZoomLevel(float level) { m_ZoomLevel = level; }
	private:
		bool OnMouseScrolled(MouseScrolledEvent& e);
		bool OnWindowResized(WindowResizeEvent& e);
	private:
		float m_AspectRatio;
		float m_ZoomLevel = 1.0f;//zoomlevel越小,图像越大，然后移动速度与它成正比
		OrthographicCamera m_Camera;
		bool m_Rotation;
		glm::vec3 m_CameraPosition = { 0.0f, 0.0f, 0.0f };
		float m_CameraRotation = 0.0f; //In degrees, in the anti-clockwise direction
		float m_CameraTranslationSpeed = 5.0f, m_CameraRotationSpeed = 180.0f;
	};
}

