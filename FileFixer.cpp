#include <algorithm>
#include <string>
#include <vector>
#include <iostream>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

// TODO: Create makefile
// TODO: Think of more things to put in here
// TODO: Possibly cmake?

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
	outFile.close();}

int main(int argc, char *argv[]) {
	SortIntoFolders();
	
	// Rudimentary flag detection
	if (argc > 1) {
		if (std::string(argv[1]) == "-m") GenerateMakeFile();
	}

	return 0;
}
