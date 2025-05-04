#include "FileTask.h"
#include "ExternalTool.h"
#include <iostream>
#include <map>
#include <thread>

FileTask::FileTask(const std::string& filePath, const std::vector<std::string>& params, ExternalTool* tool)
    : filePath_(filePath), params_(params), tool_(tool), completed_(false), result_(-1), priority_(0) {
}

FileTask::FileTask(FileTask&& other) noexcept
    : filePath_(std::move(other.filePath_)),
    params_(std::move(other.params_)),
    tool_(other.tool_),
    completed_(other.completed_.load()),
    result_(other.result_.load()),
    priority_(other.priority_.load()) {
}

FileTask& FileTask::operator=(FileTask&& other) noexcept {
    if (this != &other) {
        std::lock_guard<std::mutex> lock(mutex_);
        std::lock_guard<std::mutex> otherLock(other.mutex_);
        filePath_ = std::move(other.filePath_);
        params_ = std::move(other.params_);
        tool_ = other.tool_;
        completed_ = other.completed_.load();
        result_ = other.result_.load();
        priority_ = other.priority_.load();
    }
    return *this;
}

std::string FileTask::getFilePath() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return filePath_;
}

std::vector<std::string> FileTask::getParams() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return params_;
}

bool FileTask::isCompleted() const {
    return completed_.load();
}

int FileTask::getResult() const {
    return result_.load();
}

int FileTask::getPriority() const {
    return priority_.load();
}

void FileTask::execute() {
    if (!tool_) {
        std::cerr << "No external tool provided for " << filePath_ << std::endl;
        result_ = -1;
        return;
    }

    static const std::map<std::string, std::string> paramToCommand = {
        {"-a", "grep 'pattern' "},
        {"-b", "ls -l "},
        {"-c", "wc -l "}
    };

    std::cout << "Executing task for " << filePath_ << " in thread " << std::this_thread::get_id() << std::endl;
    for (const auto& param : params_) {
        auto it = paramToCommand.find(param);
        if (it == paramToCommand.end()) {
            std::cerr << "Unknown parameter: " << param << std::endl;
            result_ = -1;
            completed_ = false;
            return;
        }
        std::string command = it->second + filePath_;
        std::cout << "Running: " << command << std::endl;
        tool_->run(command);
    }
    result_ = 0;
    completed_ = true;
}

void FileTask::reset() {
    completed_ = false;
    result_ = -1;
}

void FileTask::setPriority(int priority) {
    priority_ = priority;
}