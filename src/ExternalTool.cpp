#include "ExternalTool.h"
#include <cstdlib>
#include <iostream>

ExternalTool::ExternalTool() : lastError_("") {}

void ExternalTool::run(const std::string& command) const {
    std::cout << "Running command: " << command << std::endl;
    int result = system(command.c_str());
    if (result != 0) {
        lastError_ = "Command failed with exit code: " + std::to_string(result);
        std::cerr << lastError_ << std::endl;
    }
    else {
        lastError_ = "";
    }
}

std::string ExternalTool::getLastError() const {
    return lastError_;
}

bool ExternalTool::isAvailable() const {
    return true;
}