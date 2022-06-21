#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "Puppet/Renderer/Texture.h"
#include "Puppet/Camera/SceneCamera.h"
#include "ScriptableEntity.h"
#include "Puppet/Mesh/Mesh.h"
#include "Puppet/Core/UUID.h"
namespace Puppet {

	struct IDComponent
	{
		UUID ID = 0;
	};

	struct TagComponent
	{
		std::string Tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag)
			: Tag(tag) {}
	};

	struct TransformComponent
	{
		//glm::mat4 Transform;
		glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };// Euler angles,radius
		glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };
		TransformComponent() {}
		TransformComponent(const TransformComponent & other) = default;
		glm::mat4 GetTransform() const
		{
			glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));

			return glm::translate(glm::mat4(1.0f), Translation)
				* rotation
				* glm::scale(glm::mat4(1.0f), Scale);
		}
	};

	struct SpriteRendererComponent
	{
		glm::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };
		Ref<Texture2D> Texture;
		float TilingFactor = 1.0f;

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const glm::vec4& color)
			: Color(color) {}
	};

	struct CameraComponent
	{
		SceneCamera Camera;
		bool Primary = true; // TODO: think about moving to Scene
		bool FixedAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
		operator SceneCamera& () { return Camera; }
		operator const SceneCamera& () const { return Camera; }
	};

	struct NativeScriptComponent
	{
		ScriptableEntity* Instance = nullptr;

		ScriptableEntity* (*InstantiateScript)();
		void (*DestroyScript)(NativeScriptComponent*);

		template<typename T>
		void Bind()
		{
			InstantiateScript = [](){ return static_cast<ScriptableEntity*>(new T()); };
			DestroyScript = [](NativeScriptComponent* nsc) { delete nsc->Instance; nsc->Instance = nullptr; };
		}
	};
	struct MeshComponent
	{
		MeshComponent() { m_Mesh = CreateRef<Mesh>(); };
		MeshComponent(const MeshComponent&) = default;
		MeshComponent(const std::string & path)
			: Path(path), m_Mesh(CreateRef<Mesh>(path))
		{
		}
		//MeshComponent(const std::filesystem::path& path)
		//	: Path(path)
		//{
		//}

		std::string Path = "None";
		Ref<Mesh> m_Mesh;
	};

	class PointLightComponent
	{
	public:
		PointLightComponent() = default;
		PointLightComponent(const PointLightComponent&) = default;
		PointLightComponent(float intensity, const glm::vec3& lightColor)
			: Intensity(intensity), LightColor(lightColor) {}

		float Intensity = 100.0f;
		glm::vec3 LightColor = { 1.0f, 1.0f, 1.0f };
	};
	class DirectionalLightComponent
	{
	public:
		DirectionalLightComponent() = default;
		DirectionalLightComponent(const DirectionalLightComponent&) = default;
		DirectionalLightComponent(float intensity)
			: Intensity(intensity) {}

		float Intensity = 1.0f;
	};
}