#pragma once
#include "Texture.h"
#include "Shader.h"
#include "Puppet/Library/TextureLibrary.h"
namespace Puppet {
    struct EnumClassHash
    {
        template <typename T>
        std::size_t operator()(T t) const
        {
            return static_cast<std::size_t>(t);
        }
    };
    enum class TextureType
    {
        Albedo = 0,
        Normal,
        Metalness,
        Roughness,
        AmbientOcclusion,
        Specular,
        Height,
        Emission,
    };
    struct MaterialTexture
    {
        Ref<Texture2D> texture2d = nullptr;
        TextureType type;
        std::string path;
    };
	class Material:public RefCounted
	{
    public:
        Material() { Initialize(); };
        Material(Ref<Shader> shader) : mShader(shader) { Initialize(); };
    public:
        void SetShader(Ref<Shader> shader) { mShader = shader; }
        [[nodiscard]] Ref<Shader> GetShader() { return mShader; }

        void AddTexture(TextureType type, Ref<Texture2D> texture)
        {
            PP_CORE_ASSERT(mTexMap.find(type) == mTexMap.end());
            mTexMap[type] = texture;
        }

        [[nodiscard]] Ref<Texture2D> GetTexture(TextureType type) { return mTexMap[type]; }
    private:
        void Initialize();
    public:
        std::vector<MaterialTexture> mTextures;

        bool bUseAlbedoMap = false;
        glm::vec4 col = { 1.0f, 1.0f, 1.0f, 1.0f }; // 0 ~ 1
        Ref<Texture2D> albedoRGBA = Texture2D::Create(1, 1);
        Ref<Texture2D> mAlbedoMap = TextureLibrary::GetInstance().Get("DefaultTexture");

        bool bUseNormalMap = false;
        Ref<Texture2D> mNormalMap = TextureLibrary::GetInstance().Get("DefaultNormal");

        bool bUseMetallicMap = false;
        float metallic = 0.1f;
        Ref<Texture2D> metallicRGBA = Texture2D::Create(1, 1);
        Ref<Texture2D> mMetallicMap = TextureLibrary::GetInstance().Get("DefaultMetallicRoughness");

        bool bUseRoughnessMap = false;
        float roughness = 0.9f;
        Ref<Texture2D> roughnessRGBA = Texture2D::Create(1, 1);
        Ref<Texture2D> mRoughnessMap = TextureLibrary::GetInstance().Get("DefaultMetallicRoughness");

        bool bUseAoMap = false;
        Ref<Texture2D> mAoMap = TextureLibrary::GetInstance().Get("WhiteTexture");
    private:
        Ref<Shader> mShader;
        std::unordered_map<TextureType, Ref<Texture2D>, EnumClassHash> mTexMap;

	};
}

