#include "BatchParser.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include <iostream>

using json = nlohmann::json;

std::vector<FileTask> BatchParser::loadTasks(const std::string& filename, ExternalTool* tool) {
    std::vector<FileTask> tasks;
    try {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cout << "Failed to open " << filename << std::endl;
            return tasks;
        }
        json j;
        file >> j;
        if (!j.is_array()) {
            std::cout << "JSON must be an array" << std::endl;
            return tasks;
        }
        for (const auto& item : j) {
            std::string filePath = item.value("filePath", "");
            std::string command = item.value("command", "");
            int priority = item.value("priority", 0);
            if (!filePath.empty() && !command.empty()) {
                tasks.emplace_back(filePath, command, tool, priority);
            }
        }
    }
    catch (const std::exception& e) {
        std::cout << "Error parsing JSON: " << e.what() << std::endl;
    }
    return tasks;
}