#include "ThreadPool.h"
#include <iostream>

ThreadPool::ThreadPool(int maxThreads) : stop_(false), maxThreads_(maxThreads), activeTasks_(0) {
    for (int i = 0; i < maxThreads_; ++i) {
        threads_.emplace_back(&ThreadPool::worker, this);
    }
}

ThreadPool::~ThreadPool() {
    stop();
}

void ThreadPool::start(std::vector<FileTask>& tasks) {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        activeTasks_ = tasks.size();
        for (auto& task : tasks) {
            task.reset(); // Сбрасываем состояние перед выполнением
            jobs_.push([&task, this]() {
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                task.execute();
                --activeTasks_;
                condition_.notify_all();
                });
        }
    }
    condition_.notify_all();
}

void ThreadPool::stop() {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        stop_ = true;
        while (!jobs_.empty()) jobs_.pop(); // Очищаем очередь
    }
    condition_.notify_all();
    for (auto& thread : threads_) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    threads_.clear();
}

bool ThreadPool::isRunning() const {
    return !stop_ && (activeTasks_ > 0 || !jobs_.empty());
}

bool ThreadPool::tasksCompleted() const {
    return jobs_.empty() && activeTasks_ == 0;
}

void ThreadPool::worker() {
    while (true) {
        std::function<void()> job;
        {
            std::unique_lock<std::mutex> lock(mutex_);
            condition_.wait(lock, [this] { return stop_ || !jobs_.empty(); });
            if (stop_ || (jobs_.empty() && activeTasks_ == 0)) return;
            job = std::move(jobs_.front());
            jobs_.pop();
        }
        job();
    }
}