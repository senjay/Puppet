#include "IconManager.h"

namespace Puppet
{
    IconManager::IconManager()
    {
        Add("DirectoryIcon", Texture2D::Create(std::string(("Resources/Icons/ContentBrowser/DirectoryIcon.png"))));
        Add("FileIcon", Texture2D::Create(std::string(("Resources/Icons/ContentBrowser/FileIcon.png"))));
        Add("PlayIcon", Texture2D::Create(std::string(("Resources/Icons/PlayButton.png"))));
        Add("StopIcon", Texture2D::Create(std::string(("Resources/Icons/StopButton.png"))));
    }
}