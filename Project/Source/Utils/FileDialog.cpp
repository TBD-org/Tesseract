#include "FileDialog.h"

#include "Application.h"

#include "imgui.h"
#include "IconsForkAwesome.h"

#include "Math/MathFunc.h"

#include <algorithm>
#include <windows.h>
#include <vector>
#include <string.h>
#include <fstream>

static char currentSelectedPath_[FILENAME_MAX];
static std::string title_;
static std::string currentDrive_;
static bool open_ = false;
static AllowedExtensionsFlag ext_;
static std::string selectedPath_;
static bool saveMode_ = false;
static char fileName_[FILENAME_MAX];
static bool override_ = false;

void FileDialog::Init(const std::string& title, bool saveMode, AllowedExtensionsFlag ext, const std::string& defaultPath) {
	title_ = title;
	if (defaultPath == "") {
		sprintf(currentSelectedPath_, GetAbsolutePath(".").c_str());
	} else {
		sprintf(currentSelectedPath_, defaultPath.c_str());
	}
	saveMode_ = saveMode;
	currentDrive_ = std::string() + currentSelectedPath_[0] + ":";
	ext_ = ext;
	override_ = false;
	memset(fileName_, 0, FILENAME_MAX);
	ImGui::OpenPopup(title.c_str());
}

bool FileDialog::OverrideAlertDialog(const std::string& file) {
	ImGui::SetNextWindowSize(ImVec2(400, 120), ImGuiCond_FirstUseEver);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20, 10));
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar;

	if (ImGui::BeginPopupModal("Override alert", nullptr, flags)) {
		std::string message = "Do you want to overwrite " + file + "?";
		ImGui::TextWrapped(message.c_str());
		ImGui::NewLine();
		ImGui::NewLine();
		ImGui::SameLine(ImGui::GetWindowWidth() - 120);
		if (ImGui::Button("Yes", ImVec2(50, 20))) {
			override_ = true;
			ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
			ImGui::PopStyleVar();
			return true;
		}

		ImGui::SameLine(ImGui::GetWindowWidth() - 60);
		if (ImGui::Button("Cancel")) {
			ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
			ImGui::PopStyleVar();
			return false;
		}

		ImGui::EndPopup();
	}
	ImGui::PopStyleVar();
	return false;
}

bool FileDialog::OpenDialog(const std::string& title, std::string& selectedPath) {
	ImGui::SetNextWindowSize(ImVec2(800, 640), ImGuiCond_FirstUseEver);
	if (ImGui::BeginPopupModal(title.c_str(), nullptr, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar)) {
		//DriversCombo
		ImGui::PushItemWidth(4 * ImGui::GetFontSize());
		if (ImGui::BeginCombo("##Drives", currentDrive_.c_str())) {
			std::vector<std::string> drives = GetDrives();
			for (std::string& drive : drives) {
				if (ImGui::Selectable(drive.c_str(), currentDrive_[0] == drive[0])) {
					currentDrive_ = drive;
					sprintf(currentSelectedPath_, drive.c_str());
				}
			}
			ImGui::EndCombo();
		}
		ImGui::PopItemWidth();

		//RouteInput
		ImGui::SameLine();
		ImGui::PushItemWidth(-1);
		ImGui::InputText("##route", currentSelectedPath_, FILENAME_MAX);
		ImGui::PopItemWidth();

		//FileExplorer
		float reserveHeight = ImGui::GetFrameHeightWithSpacing();
		ImGui::BeginChild("FileExplorer", ImVec2(0, -reserveHeight), true);
		for (std::string& file : GetFilesInFolder(currentSelectedPath_, ext_)) {
			std::string icon = ICON_FK_FILE;
			std::string absoluteFilePath = std::string(currentSelectedPath_) + "\\" + file;
			bool isDirectory = IsDirectory(absoluteFilePath.c_str());
			if (isDirectory) icon = ICON_FK_FOLDER;

			std::string selectableLabel = std::string(icon + " ") + file;
			if (ImGui::Selectable(selectableLabel.c_str())) {
				if (isDirectory) {
					if ((selectedPath_ == absoluteFilePath)) {
						if (file == "..") {
							sprintf(currentSelectedPath_, GetFileFolder(currentSelectedPath_).c_str());
						} else {
							sprintf(currentSelectedPath_, absoluteFilePath.c_str());
						}
						selectedPath_ = "";
					} else {
						selectedPath_ = absoluteFilePath.c_str();
					}
				} else {
					selectedPath_ = absoluteFilePath.c_str();
					if (saveMode_) sprintf(fileName_, GetFileNameAndExtension(absoluteFilePath.c_str()).c_str());
				}
			}
		}
		selectedPath = selectedPath_;
		ImGui::EndChild();

		//SelectedPath
		if (saveMode_) {
			ImGui::PushItemWidth(6 * ImGui::GetFontSize());
			ImGui::LabelText("##FileName", "File name:");
			ImGui::PopItemWidth();
			ImGui::SameLine();
			ImGui::PushItemWidth(25 * ImGui::GetFontSize());
			ImGui::InputText("##FileName", fileName_, FILENAME_MAX);
			ImGui::PopItemWidth();
		} else {
			ImGui::Text(selectedPath_.c_str());
		}

		//Buttons
		ImGui::SameLine(ImGui::GetWindowWidth() - 120);
		if (ImGui::Button((saveMode_) ? "Save" : "Accept", ImVec2(50, 20))) {
			selectedPath = (saveMode_) ? std::string(currentSelectedPath_) + "\\" + std::string(fileName_) : selectedPath_;

			if (!saveMode_ || !Exists(selectedPath.c_str())) {
				ImGui::CloseCurrentPopup();
				ImGui::EndPopup();
				return true;
			} else if (saveMode_) {
				ImGui::OpenPopup("Override alert");
			}
		}
		if (override_) {
			ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
			return true;
		}

		OverrideAlertDialog(GetFileNameAndExtension(selectedPath.c_str()));
		ImGui::SameLine(ImGui::GetWindowWidth() - 60);
		if (ImGui::Button("Cancel")) {
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
	return false;
}

bool FileDialog::IsDirectory(const char* path) {
	DWORD ftyp = GetFileAttributesA(path);
	return ftyp & FILE_ATTRIBUTE_DIRECTORY;
}

std::vector<std::string> FileDialog::GetDrives() {
	DWORD mask = GetLogicalDrives();
	std::vector<std::string> drives;
	for (int i = 0; i < 26; ++i) {
		if (!(mask & (1 << i))) continue;

		char letter = 'A' + i;
		std::string drive = std::string() + letter + ":";
		drives.push_back(drive);
	}

	return drives;
}

std::vector<std::string> FileDialog::GetFileExtensions(AllowedExtensionsFlag ext) {
	std::vector<std::string> filters;
	for (int i = (int) ext, pos = 0; i > 0; i >>= 1, ++pos) {
		if (i & 1) {
			filters.push_back(extensions[pos]);
		}
	}
	return filters;
}

std::vector<std::string> FileDialog::GetFilesInFolder(const char* folderPath, AllowedExtensionsFlag extFilter) {
	std::string folderPathEx = std::string(folderPath) + "\\*";
	std::vector<std::string> filePaths;
	WIN32_FIND_DATA data;
	HANDLE handle = FindFirstFile(folderPathEx.c_str(), &data);
	if (handle == INVALID_HANDLE_VALUE) return filePaths;
	bool filter = (extFilter != AllowedExtensionsFlag::ALL);
	std::vector<std::string> allowedExt = GetFileExtensions(extFilter);

	while (FindNextFile(handle, &data)) {
		if (IsDirectory((std::string(folderPath) + "\\" + data.cFileName).c_str()) || (!filter) || (std::find(allowedExt.begin(), allowedExt.end(), GetFileExtension(data.cFileName)) != allowedExt.end())) {
			filePaths.push_back(data.cFileName);
		}
	};

	FindClose(handle);
	return filePaths;
}

std::string FileDialog::GetFileNameAndExtension(const char* filePath) {
	const char* lastSlash = strrchr(filePath, '/');
	const char* lastBackslash = strrchr(filePath, '\\');
	const char* lastSeparator = Max(lastSlash, lastBackslash);

	if (lastSeparator == nullptr) {
		return filePath;
	}

	const char* fileNameAndExtension = lastSeparator + 1;
	return fileNameAndExtension;
}

std::string FileDialog::GetFileName(const char* filePath) {
	const char* lastSlash = strrchr(filePath, '/');
	const char* lastBackslash = strrchr(filePath, '\\');
	const char* lastSeparator = Max(lastSlash, lastBackslash);

	const char* fileName = lastSeparator == nullptr ? filePath : lastSeparator + 1;
	const char* lastDot = strrchr(fileName, '.');

	if (lastDot == nullptr || lastDot == fileName) {
		return fileName;
	}

	return std::string(fileName).substr(0, lastDot - fileName);
}

std::string FileDialog::GetFileExtension(const char* filePath) {
	const char* lastSlash = strrchr(filePath, '/');
	const char* lastBackslash = strrchr(filePath, '\\');
	const char* lastSeparator = Max(lastSlash, lastBackslash);
	const char* lastDot = strrchr(filePath, '.');

	if (lastDot == nullptr || lastDot == filePath || lastDot < lastSeparator || lastDot == lastSeparator + 1) {
		return std::string();
	}

	std::string extension = std::string(lastDot);
	std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
	return extension;
}

std::string FileDialog::GetFileFolder(const char* filePath) {
	const char* lastSlash = strrchr(filePath, '/');
	const char* lastBackslash = strrchr(filePath, '\\');
	const char* lastSeparator = Max(lastSlash, lastBackslash);

	if (lastSeparator == nullptr) {
		return std::string();
	}

	return std::string(filePath).substr(0, lastSeparator - filePath);
}

std::string FileDialog::GetAbsolutePath(const char* filePath) {
	char buff[FILENAME_MAX];
	GetModuleFileName(NULL, buff, FILENAME_MAX);
	return std::string(buff) + filePath;
}

inline bool FileDialog::Exists(const char* filePath) {
	std::ifstream f(filePath);
	return f.good();
}

void FileDialog::Copy(const char* path, const char* newPath) {
	CopyFile(path, newPath, false);
}