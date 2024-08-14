#include "GlobalConfig.h"
#include <core/rtc2_text_ini.h>
#include <iostream>
#include <ostream>
#include <string>
#include <iosfwd>
#include <exception>
#include <yaml-cpp/node/node.h>
#include <yaml-cpp/node/parse.h>
#include <fstream>


bool GlobalConfig::OpenFile(const std::string& filename) {
    Clear();
    std::string extension = filename.substr(filename.find_last_of('.') + 1);
    if (extension == "yaml") {
        try {
            rootNode = YAML::LoadFile(filename);
            fileType = FileType::YAML;
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Failed to open YAML file: " << e.what() << std::endl;
            return false;
        }
    } else if (extension == "ini") {
        if (rtIni.OpenFile(filename.c_str(), false)) {
            fileType = FileType::INI;
            return true;
        } else {
            std::cerr << "Failed to open INI file." << std::endl;
            return false;
        }
    } else {
        std::cerr << "Unsupported file format." << std::endl;
        return false;
    }
}

void GlobalConfig::SaveFile(const std::string& filename) {
    if (fileType == FileType::YAML) {
        try {
            std::ofstream fout(filename);
            fout << rootNode << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Failed to save YAML file: " << e.what() << std::endl;
        }
    } else if (fileType == FileType::INI) {
        rtIni.CloseFile(true);
    } else {
        std::cerr << "File type mismatch or unsupported format." << std::endl;
    }
}

void GlobalConfig::Clear() {
    if (fileType == FileType::INI) {
        rtIni.CloseFile(false);
    } else {
        rootNode = YAML::Node();
    }
}
