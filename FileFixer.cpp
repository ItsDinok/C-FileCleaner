#include <algorithm>
#include <string>
#include <vector>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <map>
#include <functional>

namespace fs = std::filesystem;

// TODOList
// TODO: Possibly cmake?

// Forward declarations for flag options
void GenerateMakeFile();
void ShowHelp();
void CleanStructures();
void RunAfterConclusion();
void ShowFileStructure();
void ListMisplaced();
void SaveTarget();
void SaveVersion();
void SaveOptimisation();

// TODO: Think of more things to put in here
const std::map<std::string, std::function<void()>> flagActions = {
	{"-m", GenerateMakeFile},
	{"--make", GenerateMakeFile},
	{"-h", ShowHelp},
	{"--help", ShowHelp},
	{"-v", [](){std::cout << "FileFixer v0.1.0\n"; }},
	{"-c", CleanStructures},
	{"--clean", CleanStructures},
	{"-r", RunAfterConclusion},
	{"--run", RunAfterConclusion},
	{"-s", ShowFileStructure},
	{"--structure", ShowFileStructure},
	{"--list-misplaced", ListMisplaced},

	// These require things after their declaration
	{"--target", SaveTarget},
	{"--std", SaveVersion},
	{"--optimisation", SaveOptimisation}
};

// TODO: Think of more things to put in here
const std::vector<std::pair<std::string, std::vector<std::string>>> fileMappings = {
	{"src",			{".cpp", ".c", ".o"}},
	{"include",		{".h", ".hpp"}},
	{"lib",			{".dll", ".so", ".a"}},
	{"assets",		{".png", ".jpg", ".mp3"}},
	{"text",		{".txt"}}
};

std::map<std::string, std::string> programState = {
	{"mainFolder", ""},
	{"optimisationLevel", ""},
	{"c++Version", "17"},

	// Bools
	{"listMisplaced", "false"},
	{"showStructure", "false"},
	{"runAfterConclusion", "false"},
	{"cleanStructures", "false"}
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
void GenerateMakeFile() {
	// Create the bin directory
	fs::create_directory("bin");

	std::vector<std::string> makefileLines;

	makefileLines.push_back("CXX = g++");

	std::string cxxFlags = "CXXFLAGS = -Wall -Wextra -Iinclude";
	if (programState["optimisationLevel"] != "") cxxFlags += programState["optimisationLevel"];
	cxxFlags+=(programState["c++Version"]);
	makefileLines.push_back(cxxFlags);

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
	std::cout << "------------------------------------------------------------" << std::endl;
	std::cout << "FileFixer help:" << std::endl;
	std::cout << "FileFixer: Restructures file system" << std::endl;
	std::cout << "-m: Generates make file, works as a flag" << std::endl;
	std::cout << "--make: Generates make file, works as a flag" << std::endl;
	std::cout << "-h: Shows help menu, but you knew that anyway" << std::endl;
	std::cout << "-v: Shows version information" << std::endl;
	std::cout << "------------------------------------------------------------" << std::endl;
}

// This deletes files such as build and obj files
// Similar to a makefile clean build
void CleanStructures() {
}

// This runs the built item after conclusion
void RunAfterConclusion() {
}

void ShowFileStructure() {
}

void SaveOptimisation() {
}

void SaveVersion() {
}

void SaveTarget() {
}

void ListMisplaced() {
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
