#pragma once

#include <filesystem>

namespace NanoCore {

	struct ProjectConfig
	{
		std::string Name;

		std::string AssetDirectory;
		std::string AssetRegistryPath;

		std::string AudioCommandsRegistryPath = "";

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
		std::string ProjectDirectory = "resources";
	};

	class Project : public RefCount
	{
	public:
		Project();
		Project(ProjectConfig);
		~Project();

		const ProjectConfig& GetConfig() const { return m_Config; }

		static Shared<Project> GetActive() { return s_ActiveProject; }
		static void SetActive(Shared<Project> project);

		static const std::string& GetProjectName()
		{
			NANO_ENGINE_LOG_ASSERT(s_ActiveProject);
			return s_ActiveProject->GetConfig().Name;
		}

		static std::filesystem::path GetProjectDirectory()
		{
			NANO_ENGINE_LOG_ASSERT(s_ActiveProject);
			return s_ActiveProject->GetConfig().ProjectDirectory;
		}

		static std::filesystem::path GetAssetDirectory()
		{
			NANO_ENGINE_LOG_ASSERT(s_ActiveProject);
			return std::filesystem::path(s_ActiveProject->GetConfig().ProjectDirectory) / s_ActiveProject->GetConfig().AssetDirectory;
		}

		static std::filesystem::path GetAssetRegistryPath()
		{
			NANO_ENGINE_LOG_ASSERT(s_ActiveProject);
			return std::filesystem::path(s_ActiveProject->GetConfig().ProjectDirectory) / s_ActiveProject->GetConfig().AssetRegistryPath;
		}

		static std::filesystem::path GetMeshPath()
		{
			NANO_ENGINE_LOG_ASSERT(s_ActiveProject);
			return std::filesystem::path(s_ActiveProject->GetConfig().ProjectDirectory) / s_ActiveProject->GetConfig().MeshPath;
		}

		static std::filesystem::path GetAudioCommandsRegistryPath()
		{
			NANO_ENGINE_LOG_ASSERT(s_ActiveProject);
			return std::filesystem::path(s_ActiveProject->GetConfig().ProjectDirectory) / s_ActiveProject->GetConfig().AudioCommandsRegistryPath;
		}

		static std::filesystem::path GetScriptModulePath()
		{
			NANO_ENGINE_LOG_ASSERT(s_ActiveProject);
			return std::filesystem::path(s_ActiveProject->GetConfig().ProjectDirectory) / s_ActiveProject->GetConfig().ScriptModulePath;
		}

		static std::filesystem::path GetScriptModuleFilePath()
		{
			NANO_ENGINE_LOG_ASSERT(s_ActiveProject);
			return GetScriptModulePath() / fmt::format("{0}.dll", GetProjectName());
		}

		static std::filesystem::path GetCacheDirectory()
		{
			NANO_ENGINE_LOG_ASSERT(s_ActiveProject);
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
