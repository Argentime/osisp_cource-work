#include "ExternalTool.h"
#include <cstdlib>
#include <array>
#include <iostream>
#include <algorithm>
#include <vector>

ExternalTool::ExternalTool() : lastError_("No errors") {}

bool ExternalTool::isCommandSafe(const std::string& command) const {
    static const std::vector<std::string> forbidden = { "rm", "del", "format", "mkfs", "sudo" };
    std::string lowerCommand = command;
    std::transform(lowerCommand.begin(), lowerCommand.end(), lowerCommand.begin(), ::tolower);
    return std::none_of(forbidden.begin(), forbidden.end(),
        [&lowerCommand](const auto& word) { return lowerCommand.find(word) != std::string::npos; });
}

void ExternalTool::run(std::string command) {
    if (!isCommandSafe(command)) {
        lastError_ = "Unsafe command detected";
        return;
    }
    int result = std::system(command.c_str());
    lastError_ = result == 0 ? "No errors" : "Command failed with code " + std::to_string(result);
}

std::string ExternalTool::runWithOutput(std::string command) {
    if (!isCommandSafe(command)) {
        lastError_ = "Unsafe command detected";
        return "";
    }
    std::array<char, 128> buffer;
    std::string result;
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        lastError_ = "Failed to open pipe";
        return "";
    }
    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        result += buffer.data();
    }
    int status = pclose(pipe);
    lastError_ = status == 0 ? "No errors" : "Command failed with code " + std::to_string(status);
    return result;
}

std::string ExternalTool::getLastError() const { return lastError_; }

bool ExternalTool::isAvailable() const { return true; }