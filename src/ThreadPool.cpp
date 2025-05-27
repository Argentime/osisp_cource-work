#include "ThreadPool.h"
#include <iostream>
#include <algorithm>

ThreadPool::ThreadPool(int maxThreads) : maxThreads_(maxThreads > 0 ? maxThreads : 1), stop_(false), activeTasks_(0), tasksPtr_(nullptr) {
    for (int i = 0; i < maxThreads_; ++i) {
        threads_.emplace_back(&ThreadPool::worker, this);
    }
}

ThreadPool::~ThreadPool() { stop(); }

void ThreadPool::worker() {
    while (!stop_) {
        size_t taskIndex = 0;
        {
            std::unique_lock<std::mutex> lock(mutex_);
            condition_.wait(lock, [this] { return stop_ || !jobs_.empty(); });
            if (stop_ && jobs_.empty()) break;
            taskIndex = jobs_.top().second;
            jobs_.pop();
        }
        if (tasksPtr_ && taskIndex < tasksPtr_->size()) {
            (*tasksPtr_)[taskIndex].execute();
            activeTasks_--;
            condition_.notify_all();
        }
    }
}

void ThreadPool::start(std::vector<FileTask>& tasks) {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        tasksPtr_ = &tasks; // Сохраняем указатель на tasks_
        activeTasks_ = tasks.size();
        for (size_t i = 0; i < tasks.size(); ++i) {
            tasks[i].reset();
            jobs_.push({ tasks[i].getPriority(), i }); // Добавляем приоритет и индекс
        }
    }
    condition_.notify_all();
}

void ThreadPool::stop() {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        stop_ = true;
        while (!jobs_.empty()) jobs_.pop();
        tasksPtr_ = nullptr;
    }
    condition_.notify_all();
    for (auto& thread : threads_) {
        if (thread.joinable()) thread.join();
    }
    threads_.clear();
}

bool ThreadPool::isRunning() const { return !stop_ && (activeTasks_ > 0 || !jobs_.empty()); }
bool ThreadPool::tasksCompleted() const { return jobs_.empty() && activeTasks_ == 0; }