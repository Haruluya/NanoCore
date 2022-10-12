#pragma once

#include <filesystem>

namespace NanoCore {

	struct ProjectConfig
	{
		std::string Name;

		std::string AssetDirectory;
		std::string AssetRegistryPath;

		std::string AudioCommandsRegistryPath = "Assets/AudioCommandsRegistry.RAr";

		std::string MeshPath;
		std::string MeshSourcePath;

		std::string ScriptModulePath;
		std::string DefaultNamespace;

		std::string StartScene;

		bool ReloadAssemblyOnPlay;

		bool EnableAutoSave = false;
		int AutoSaveIntervalSeconds = 300;

		// Not serialized
		std::string ProjectFileName;
		std::string ProjectDirectory;
	};

	class Project : public RefCount
	{
	public:
		Project();
		~Project();

		const ProjectConfig& GetConfig() const { return m_Config; }

		static Shared<Project> GetActive() { return s_ActiveProject; }
		static void SetActive(Shared<Project> project);

		static const std::string& GetProjectName()
		{
			RA_CORE_ASSERT(s_ActiveProject);
			return s_ActiveProject->GetConfig().Name;
		}

		static std::filesystem::path GetProjectDirectory()
		{
			RA_CORE_ASSERT(s_ActiveProject);
			return s_ActiveProject->GetConfig().ProjectDirectory;
		}

		static std::filesystem::path GetAssetDirectory()
		{
			RA_CORE_ASSERT(s_ActiveProject);
			return std::filesystem::path(s_ActiveProject->GetConfig().ProjectDirectory) / s_ActiveProject->GetConfig().AssetDirectory;
		}

		static std::filesystem::path GetAssetRegistryPath()
		{
			RA_CORE_ASSERT(s_ActiveProject);
			return std::filesystem::path(s_ActiveProject->GetConfig().ProjectDirectory) / s_ActiveProject->GetConfig().AssetRegistryPath;
		}

		static std::filesystem::path GetMeshPath()
		{
			RA_CORE_ASSERT(s_ActiveProject);
			return std::filesystem::path(s_ActiveProject->GetConfig().ProjectDirectory) / s_ActiveProject->GetConfig().MeshPath;
		}

		static std::filesystem::path GetAudioCommandsRegistryPath()
		{
			RA_CORE_ASSERT(s_ActiveProject);
			return std::filesystem::path(s_ActiveProject->GetConfig().ProjectDirectory) / s_ActiveProject->GetConfig().AudioCommandsRegistryPath;
		}

		static std::filesystem::path GetScriptModulePath()
		{
			RA_CORE_ASSERT(s_ActiveProject);
			return std::filesystem::path(s_ActiveProject->GetConfig().ProjectDirectory) / s_ActiveProject->GetConfig().ScriptModulePath;
		}

		static std::filesystem::path GetScriptModuleFilePath()
		{
			RA_CORE_ASSERT(s_ActiveProject);
			return GetScriptModulePath() / fmt::format("{0}.dll", GetProjectName());
		}

		static std::filesystem::path GetCacheDirectory()
		{
			RA_CORE_ASSERT(s_ActiveProject);
			return std::filesystem::path(s_ActiveProject->GetConfig().ProjectDirectory) / "Cache";
		}
	private:
		void OnDeserialized();
	private:
		ProjectConfig m_Config;

		friend class ProjectSettingsWindow;
		friend class ProjectSerializer;

		inline static Shared<Project> s_ActiveProject;
	};

}
