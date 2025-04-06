#include "FileTask.h"
#include "ExternalTool.h"
#include <iostream>

FileTask::FileTask(const std::string& filePath, const std::string& command, ExternalTool* tool)
    : filePath_(filePath), command_(command), tool_(tool), completed_(false), result_(-1), priority_(0) {
}

std::string FileTask::getFilePath() const { return filePath_; }
std::string FileTask::getCommand() const { return command_; }
bool FileTask::isCompleted() const { return completed_; }
int FileTask::getResult() const { return result_; }
int FileTask::getPriority() const { return priority_; }

void FileTask::execute() {
    if (!tool_) {
        std::cerr << "No external tool provided for " << filePath_ << std::endl;
        return;
    }
    std::cout << "Executing task for " << filePath_ << std::endl;
    tool_->run(command_);
    result_ = 0; // Пока заглушка, позже добавим реальный результат
    completed_ = true;
}

void FileTask::reset() {
    completed_ = false;
    result_ = -1;
}

void FileTask::setPriority(int priority) {
    priority_ = priority;
}