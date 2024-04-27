//
// Created by dicta on 27.04.2024.
//

#include <utility>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>

#include "Database.h"

Database::Database(const std::string& name) {
    // Create the directory path and the file path
    std::filesystem::path dirPath = name;
    std::filesystem::path filePath = dirPath / (name + ".conf");

    std::cout << "Initialization database with name " << name << std::endl;

    // Check if the directory exists, if not create it
    if (!std::filesystem::exists(dirPath)) {
        std::filesystem::create_directory(dirPath);
        std::cout << "Created directory: " << dirPath << std::endl;
    }

    // Check if the file exists
    if (!std::filesystem::exists(filePath)) {
        // File does not exist, create a new file
        std::ofstream file(filePath);  // Creating and closing the file
        std::cout << "Created configuration file: " << filePath << std::endl;
    } else {
        std::ifstream file(filePath);
        std::string line;
        while (getline(file, line)) {
            std::cout << line << std::endl;
        }
        file.close();
    }
}
