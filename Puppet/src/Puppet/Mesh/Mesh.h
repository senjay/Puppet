#pragma once
#include <glm/glm.hpp>
#include "Puppet/Renderer/Pipeline.h"
#include "Puppet/Renderer/IndexBuffer.h"
#include "Puppet/Renderer/VertexBuffer.h"

#include "Puppet/Renderer/Shader.h"
#include "Puppet/Renderer/Texture.h"
#include "Puppet/Renderer/Material.h"
#include "Puppet/Library/ShaderLibrary.h"
#include "Puppet/Mesh/SubMesh.h"

#include "Puppet/Animation/Animadata.h"
#include "Puppet/Animation/Animator.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <optional>
#include <map>
namespace Puppet {

	class Mesh:public RefCounted
	{
	public:
		Mesh() = default;
		Mesh(const Mesh&) = default;
		Mesh(const std::string& path)
		{
			Ref<Shader>r = CreateRef<Material>(ShaderLibrary::GetInstance().Get("Default"));
			m_Material.push_back(r);
			LoadModel(path);
		}

		Mesh(const std::string& path, Ref<Shader> shader)
		{
			m_Material.push_back(CreateRef<Material>(shader));
			LoadModel(path);
		}

		void SetShader(Ref<Shader> shader) { m_Material[0]->SetShader(shader); };
		void Draw(const glm::mat4& transform, const glm::vec3& cameraPos, int entityID);
		void Draw(const glm::mat4& transform, const glm::vec3& cameraPos, Ref<Shader> shader, int entityID);

		void Draw();

		auto& GetBoneInfoMap() { return mBoneInfoMap; }
		int& GetBoneCount() { return mBoneCounter; }
	private:
		void LoadModel(const std::string& path);
		void ProcessNode(aiNode* node, const aiScene* scene, uint32_t& subMeshIndex);

		template <typename Vertex>
		SubMesh ProcessMesh(aiMesh* mesh, const aiScene* scene, uint32_t& subMeshIndex);
		std::optional<std::vector<MaterialTexture>> loadMaterialTextures(aiMaterial* mat, aiTextureType type, uint32_t& subMeshIndex);
	public:
		// Animation
		bool bAnimated = false;
		bool bPlayAnim = false;
		bool bStopAnim = false;

		Animation m_Animation;
		Animator m_Animator;

		float mAnimPlaySpeed = 1.0f;

		std::vector<Ref<Material>> m_Material;
		std::vector<SubMesh> mSubMeshes;
	private:
		std::string mDirectory;

		// Animation
		int mBoneCounter = 0;
		std::map<std::string, BoneInfo> mBoneInfoMap;
	};
}

