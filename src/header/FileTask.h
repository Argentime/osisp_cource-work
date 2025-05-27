#ifndef FILETASK_H
#define FILETASK_H

#include <string>
#include <vector>
#include <atomic>
#include <mutex>
#include "ExternalTool.h"

class FileTask {
private:
    std::string filePath_;
    std::string command_; // Произвольная команда
    std::string output_;  // Вывод команды
    ExternalTool* tool_;
    std::atomic<bool> completed_;
    std::atomic<int> result_;
    std::atomic<int> priority_;
    std::mutex mutex_;

public:
    FileTask(std::string filePath, std::string command, ExternalTool* tool, int priority = 0);
    FileTask(FileTask&&) noexcept;
    FileTask& operator=(FileTask&&) noexcept;
    FileTask(const FileTask&) = delete;
    FileTask& operator=(const FileTask&) = delete;

    std::string getFilePath() const;
    std::string getCommand() const;
    std::string getOutput() const;
    bool isCompleted() const;
    int getResult() const;
    void execute();
    void reset();
    void setPriority(int priority);
    int getPriority() const;

    // Для priority_queue
    bool operator<(const FileTask& other) const {
        return priority_ < other.priority_;
    }
};

#endif