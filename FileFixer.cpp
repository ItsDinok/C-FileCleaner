#include <algorithm>
#include <string>
#include <vector>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

// TODO: Create makefile
// TODO: Think of more things to put in here
const std::vector<std::pair<std::string, std::vector<std::string>>> fileMappings = {
	{"src",			{".cpp", ".c", ".o"}},
	{"include",		{".h", ".hpp"}},
	{"lib",			{".dll", ".so", ".a"}},
	{"assets",		{".png", ".jpg", ".mp3"}},
	{"text",		{".txt"}}
};

std::string GetFolderForFile(const std::string& ext) {
	for (const auto& mapping : fileMappings) {
		const std::string& folder = mapping.first;
		const std::vector<std::string>& extensions = mapping.second;

		for (const auto& validExtension : extensions) {
			if (ext == validExtension) return folder;
		}
	}
	return "-1";
}

void Move(const std::string& ext, fs::directory_entry item) {
	fs::path destFolder = fs::current_path() / ext;

	if (!fs::exists(destFolder)) fs::create_directory(destFolder);
	
	fs::path destPath = destFolder / item.path().filename();
	fs::rename(item.path(), destPath);
}

void SortIntoFolders() {
	for (const auto& item : fs::directory_iterator(".")) {
		const std::string extension = item.path().extension().string();
		const std::string file = GetFolderForFile(extension);
		
		if (file != "-1") Move(file, item);
	}
}

int main() {
	SortIntoFolders();

	return 0;
}
