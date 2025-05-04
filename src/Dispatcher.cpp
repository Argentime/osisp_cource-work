#include "Dispatcher.h"
#include <sstream>
#include <iostream>

Dispatcher::Dispatcher() : maxThreads_(1), threadPool_(std::make_unique<ThreadPool>(maxThreads_)) {}

Dispatcher::~Dispatcher() {
    threadPool_->stop();
}

void Dispatcher::addTask(FileTask&& task) {
    tasks_.push_back(std::move(task));
}

void Dispatcher::runTasks() {
    if (tasks_.empty()) {
        std::cout << "No tasks to run.\n";
        return;
    }
    if (!isThreadPoolActive()) {
        threadPool_ = std::make_unique<ThreadPool>(maxThreads_);
    }
    std::cout << "Running " << tasks_.size() << " tasks in parallel...\n";
    threadPool_->start(tasks_);
    while (threadPool_->isRunning()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    threadPool_->stop();
    std::cout << "All tasks completed.\n";
}

void Dispatcher::clearTasks() {
    tasks_.clear();
    if (isThreadPoolActive()) {
        threadPool_->stop();
        threadPool_ = std::make_unique<ThreadPool>(maxThreads_);
    }
}

size_t Dispatcher::getTaskCount() const {
    return tasks_.size();
}

size_t Dispatcher::getCompletedCount() const {
    size_t count = 0;
    for (const auto& task : tasks_) {
        if (task.isCompleted()) ++count;
    }
    return count;
}

void Dispatcher::setMaxThreads(int maxThreads) {
    if (maxThreads > 0) {
        maxThreads_ = maxThreads;
        if (isThreadPoolActive()) {
            threadPool_->stop();
        }
        threadPool_ = std::make_unique<ThreadPool>(maxThreads_);
        std::cout << "Max threads set to: " << maxThreads_ << std::endl;
    }
}

std::string Dispatcher::getTaskList() const {
    std::stringstream ss;
    ss << "Tasks (" << tasks_.size() << "):\n";
    for (size_t i = 0; i < tasks_.size(); ++i) {
        ss << i + 1 << ". File: " << tasks_[i].getFilePath() << ", Params: ";
        for (const auto& param : tasks_[i].getParams()) {
            ss << param << " ";
        }
        ss << ", Completed: " << (tasks_[i].isCompleted() ? "Yes" : "No")
            << ", Priority: " << tasks_[i].getPriority() << "\n";
    }
    return ss.str();
}

std::string Dispatcher::getSummary() const {
    std::stringstream ss;
    ss << "Summary:\n";
    ss << "Total tasks: " << getTaskCount() << "\n";
    ss << "Completed tasks: " << getCompletedCount() << "\n";
    ss << "Pending tasks: " << (getTaskCount() - getCompletedCount()) << "\n";
    return ss.str();
}

bool Dispatcher::tasksCompleted() const {
    return threadPool_->tasksCompleted();
}

bool Dispatcher::isThreadPoolActive() const {
    return threadPool_ && threadPool_->isRunning();
}