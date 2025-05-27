#include "FileTask.h"
#include <iostream>
#include <thread>

FileTask::FileTask(std::string filePath, std::string command, ExternalTool* tool, int priority)
    : filePath_(std::move(filePath)), command_(std::move(command)), tool_(tool),
    completed_(false), result_(-1), priority_(priority) {
}

FileTask::FileTask(FileTask&& other) noexcept
    : filePath_(std::move(other.filePath_)), command_(std::move(other.command_)),
    output_(std::move(other.output_)), tool_(other.tool_),
    completed_(other.completed_.load()), result_(other.result_.load()),
    priority_(other.priority_.load()) {
    other.tool_ = nullptr;
}

FileTask& FileTask::operator=(FileTask&& other) noexcept {
    if (this != &other) {
        std::lock_guard<std::mutex> lock(mutex_);
        filePath_ = std::move(other.filePath_);
        command_ = std::move(other.command_);
        output_ = std::move(other.output_);
        tool_ = other.tool_;
        completed_ = other.completed_.load();
        result_ = other.result_.load();
        priority_ = other.priority_.load();
        other.tool_ = nullptr;
    }
    return *this;
}

std::string FileTask::getFilePath() const { return filePath_; }
std::string FileTask::getCommand() const { return command_; }
std::string FileTask::getOutput() const { return output_; }
bool FileTask::isCompleted() const { return completed_; }
int FileTask::getResult() const { return result_; }
int FileTask::getPriority() const { return priority_; }

void FileTask::execute() {
    if (!tool_ || completed_) return;
    std::lock_guard<std::mutex> lock(mutex_);
    std::cout << "Executing task for " << filePath_ << " in thread " << std::this_thread::get_id() << std::endl;
    output_ = tool_->runWithOutput(command_ + " " + filePath_);
    if (!output_.empty()) {
        std::cout << "[Output for " << filePath_ << "]:\n" << output_ << std::endl;
    }
    else {
        std::cout << "[Output for " << filePath_ << "]: No output or error occurred\n";
    }
    result_ = output_.empty() ? -1 : 0;
    completed_ = true;
}
void FileTask::reset() {
    std::lock_guard<std::mutex> lock(mutex_);
    completed_ = false;
    result_ = -1;
    output_.clear();
}

void FileTask::setPriority(int priority) {
    priority_ = priority;
}