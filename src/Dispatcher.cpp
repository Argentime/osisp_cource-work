#include "Dispatcher.h"
#include <iostream>
#include <thread>
#include <chrono>

Dispatcher::Dispatcher() : maxThreads_(1), threadPool_(std::make_unique<ThreadPool>(maxThreads_)) {}

Dispatcher::~Dispatcher() { clearTasks(); }

void Dispatcher::addTask(FileTask&& task) {
    tasks_.push_back(std::move(task));
    std::cout << "Task added: " << tasks_.back().getFilePath() << std::endl;
}

void Dispatcher::runTasks() {
    if (tasks_.empty()) {
        std::cout << "No tasks to run" << std::endl;
        return;
    }
    std::cout << "Running " << tasks_.size() << " tasks in parallel..." << std::endl;
    if (!isThreadPoolActive()) {
        threadPool_ = std::make_unique<ThreadPool>(maxThreads_);
    }
    threadPool_->start(tasks_);
    while (threadPool_->isRunning()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    threadPool_->stop();
    std::cout << "All tasks completed." << std::endl;
}

void Dispatcher::clearTasks() {
    tasks_.clear();
    threadPool_->stop();
    threadPool_ = std::make_unique<ThreadPool>(maxThreads_);
}

size_t Dispatcher::getTaskCount() const { return tasks_.size(); }

size_t Dispatcher::getCompletedCount() const {
    return std::count_if(tasks_.begin(), tasks_.end(),
        [](const auto& task) { return task.isCompleted(); });
}

void Dispatcher::setMaxThreads(int maxThreads) {
    if (maxThreads > 0) {
        maxThreads_ = maxThreads;
        threadPool_->stop();
        threadPool_ = std::make_unique<ThreadPool>(maxThreads_);
        std::cout << "Max threads set to: " << maxThreads_ << std::endl;
    }
}

std::string Dispatcher::getTaskList() const {
    std::string result = "Tasks:\n";
    for (size_t i = 0; i < tasks_.size(); ++i) {
        result += std::to_string(i + 1) + ". File: " + tasks_[i].getFilePath() +
            ", Command: " + tasks_[i].getCommand() +
            ", Priority: " + std::to_string(tasks_[i].getPriority()) +
            ", Status: " + (tasks_[i].isCompleted() ? "Completed" : "Pending") +
            ", Output: " + tasks_[i].getOutput() + "\n";
    }
    return result;
}

std::string Dispatcher::getSummary() const {
    size_t completed = getCompletedCount();
    return "Summary:\nTotal tasks: " + std::to_string(tasks_.size()) +
        "\nCompleted tasks: " + std::to_string(completed) +
        "\nPending tasks: " + std::to_string(tasks_.size() - completed);
}

bool Dispatcher::tasksCompleted() const { return getCompletedCount() == tasks_.size(); }

bool Dispatcher::isThreadPoolActive() const { return threadPool_ && threadPool_->isRunning(); }

ExternalTool* Dispatcher::getTool() { return &tool_; }