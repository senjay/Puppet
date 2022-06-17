#include "PPpch.h"
#include "ConfigManager.h"
#define ENGINE_ROOT_DIR "E:/HEngine/build/bin"
namespace Puppet
{
    glm::vec2 ConfigManager::mViewportSize = { 1280.0f, 720.0f };
    int ConfigManager::selectedEntity = -1;

    ConfigManager::ConfigManager()
    {
        m_RootFolder = { PP_EXPAND_MACRO(ENGINE_ROOT_DIR) };
        m_AssetsFolder = m_RootFolder / "Assets";
        m_ShadersFolder = m_RootFolder / "Shaders";
        m_ResourcesFolder = m_RootFolder / "Resources";
    }

    void ConfigManager::Clear()
    {
        m_RootFolder.clear();
        m_AssetsFolder.clear();
        m_ShadersFolder.clear();
        m_ResourcesFolder.clear();
    }

    const std::filesystem::path& ConfigManager::GetRootFolder() const
    {
        PP_CORE_ASSERT(std::filesystem::exists(m_RootFolder));
        return m_RootFolder;
    }

    const std::filesystem::path& ConfigManager::GetAssetsFolder() const
    {
        PP_CORE_ASSERT(std::filesystem::exists(m_AssetsFolder));
        return m_AssetsFolder;
    }

    const std::filesystem::path& ConfigManager::GetShadersFolder() const
    {
        PP_CORE_ASSERT(std::filesystem::exists(m_ShadersFolder));
        return m_ShadersFolder;
    }

    const std::filesystem::path& ConfigManager::GetResourcesFolder() const
    {
        PP_CORE_ASSERT(std::filesystem::exists(m_ResourcesFolder));
        return m_ResourcesFolder;
    }
}