#pragma once
#include <filesystem>
#include <optional>
namespace Puppet {

	class ContentBrowserPanel
	{
	public:
		ContentBrowserPanel();

		void OnImGuiRender(bool* pOpen);
	private:
		void DrawTree();
		void DrawTreeRecursive(const std::filesystem::path& currentPath);
		void DrawContent();
	private:
		std::filesystem::path m_CurrentDirectory;
		std::optional<std::filesystem::path> m_SelectedDirectory;
	};
}