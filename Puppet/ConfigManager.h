#pragma once
#include "Puppet/Core/BaseSingleton.h"

#include <filesystem>
#include <glm/glm.hpp>

namespace Puppet
{
    class ConfigManager final : public BaseSingleton<ConfigManager>
    {
    public:
        ConfigManager();
        ConfigManager(const ConfigManager&) = delete;
        ConfigManager& operator=(const ConfigManager&) = delete;

    public:
        void Clear();

        const std::filesystem::path& GetRootFolder() const;
        const std::filesystem::path& GetAssetsFolder() const;
        const std::filesystem::path& GetShadersFolder() const;
        const std::filesystem::path& GetResourcesFolder() const;

        static glm::vec2 mViewportSize;
        static int selectedEntity;
    private:
        std::filesystem::path m_RootFolder;
        std::filesystem::path m_EditorFolder;
        std::filesystem::path m_AssetsFolder;
        std::filesystem::path m_ShadersFolder;
        std::filesystem::path m_ResourcesFolder;
    };
}
