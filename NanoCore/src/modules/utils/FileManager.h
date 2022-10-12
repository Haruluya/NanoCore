#pragma once

#include "modules/rendering/Buffer.h"

#ifdef CreateDirectory
#undef CreateDirectory
#undef DeleteFile
#undef MoveFile
#undef CopyFile
#undef SetEnvironmentVariable
#undef GetEnvironmentVariable
#endif

#include <functional>
#include <filesystem>

namespace NanoCore {

	enum class FileSystemAction
	{
		Added, Rename, Modified, Delete
	};

	struct FileSystemChangedEvent
	{
		FileSystemAction Action;
		std::filesystem::path FilePath;
		bool IsDirectory;

		// If this is a rename event the new name will be in the FilePath
		std::wstring OldName = L"";
	};

	class FileSystem
	{
	public:
		static bool CreateDirectory(const std::filesystem::path& directory);
		static bool CreateDirectory(const std::string& directory);
		static bool Exists(const std::filesystem::path& filepath);
		static bool Exists(const std::string& filepath);
		static bool DeleteFile(const std::filesystem::path& filepath);
		static bool MoveFile(const std::filesystem::path& filepath, const std::filesystem::path& dest);
		static bool CopyFile(const std::filesystem::path& filepath, const std::filesystem::path& dest);
		static bool IsDirectory(const std::filesystem::path& filepath);

		static bool Move(const std::filesystem::path& oldFilepath, const std::filesystem::path& newFilepath);
		static bool Copy(const std::filesystem::path& oldFilepath, const std::filesystem::path& newFilepath);
		static bool Rename(const std::filesystem::path& oldFilepath, const std::filesystem::path& newFilepath);
		static bool RenameFilename(const std::filesystem::path& oldFilepath, const std::string& newName);

		static bool ShowFileInExplorer(const std::filesystem::path& path);
		static bool OpenDirectoryInExplorer(const std::filesystem::path& path);
		static bool OpenExternally(const std::filesystem::path& path);

		static bool WriteBytes(const std::filesystem::path& filepath, const Buffer& buffer);
		static Buffer ReadBytes(const std::filesystem::path& filepath);
	public:
		using FileSystemChangedCallbackFn = std::function<void(const std::vector<FileSystemChangedEvent>&)>;

		static void SetChangeCallback(const FileSystemChangedCallbackFn& callback);
		static void StartWatching();
		static void StopWatching();

		static std::filesystem::path OpenFileDialog(const char* filter = "All\0*.*\0");
		static std::filesystem::path OpenFolderDialog(const char* initialFolder = "");
		static std::filesystem::path SaveFileDialog(const char* filter = "All\0*.*\0");

		static void SkipNextFileSystemChange();

	public:
		static bool HasEnvironmentVariable(const std::string& key);
		static bool SetEnvironmentVariable(const std::string& key, const std::string& value);
		static std::string GetEnvironmentVariable(const std::string& key);

	private:
		static unsigned long Watch(void* param);

	private:
		static FileSystemChangedCallbackFn s_Callback;
	};
}
