#include "Dispatcher.h"
#include <iostream>

Dispatcher::Dispatcher() : maxThreads_(1) {}

void Dispatcher::addTask(const FileTask& task) {
    tasks_.push_back(task);
}

void Dispatcher::runTasks() {
    std::cout << "Running " << tasks_.size() << " tasks...\n";
    for (auto& task : tasks_) {
        task.execute(); // Пока последовательно
    }
}

void Dispatcher::clearTasks() {
    tasks_.clear();
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
        std::cout << "Max threads set to: " << maxThreads_ << std::endl;
    }
}