#include <algorithm>
#include <string>
#include <vector>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <map>
#include <functional>

namespace fs = std::filesystem;

// TODO: Think of more things to put in here
// TODO: Possibly cmake?


// Forward declarations for flag options
void GenerateMakeFile();
void ShowHelp();

const std::map<std::string, std::function<void()>> flagActions = {
	{"-m", GenerateMakeFile},
	{"--make", GenerateMakeFile},
	{"-h", ShowHelp},
	{"--help", ShowHelp},
	{"-v", [](){std::cout << "FileFixer v0.1.0\n"; }}
};

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

// This function requires the enabling of a flag -m
// TODO: More complex makefiles (account for all fileMappings)
// NOTE: Currently a flag manager IS NOT in use, if multiple flags are needed one will be used
void GenerateMakeFile() {
	// Create the bin directory
	fs::create_directory("bin");

	std::vector<std::string> makefileLines;

	makefileLines.push_back("CXX = g++");
	makefileLines.push_back("CXXFLAGS = -Wall -Wextra -Iinclude -std=c++17");

	makefileLines.push_back("SRC = $(wildcard src/*.cpp)");
	makefileLines.push_back("OBJ = $(SRC:src/%.cpp=build/%.o)");
	makefileLines.push_back("TARGET = bin/my_app");
	makefileLines.push_back("");
	makefileLines.push_back("all: $(TARGET)");
	makefileLines.push_back("");
	makefileLines.push_back("$(TARGET): $(OBJ)");
	makefileLines.push_back("\t$(CXX) $(OBJ) -o $(TARGET)");
	makefileLines.push_back("");
	makefileLines.push_back("build/%.o: src/%.cpp");
	makefileLines.push_back("\tmkdir -p build");
	makefileLines.push_back("\t$(CXX) $(CXXFLAGS) -c $< -o $@");
	makefileLines.push_back("");
	makefileLines.push_back("clean:");
	makefileLines.push_back("\trm -rf build/* $(TARGET)");
	makefileLines.push_back("");
	makefileLines.push_back(".PHONY: all clean");

	// Write to file
	std::ofstream outFile("Makefile");
	for (const auto& line : makefileLines) {
		outFile << line << "\n";
	}
	outFile.close();
}

void ShowHelp() {

}

// Flag manager
// TODO: Flag dependencies
void ParseFlags(int argc, char* argv[]) {
	// No arguments provided, default behaviour
	if (argc >= 2) return;

	// Ignore invalid flags
	for (int i = 1; i < argc; ++i) {
		std::string flag = argv[i];
		auto it  = flagActions.find(flag);

		if (it != flagActions.end()) it->second();
	}
}

int main(int argc, char *argv[]) {
	ParseFlags(argc, argv);

	SortIntoFolders();

	return 0;
}
