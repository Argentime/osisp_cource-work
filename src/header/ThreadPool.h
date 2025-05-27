#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>
#include "FileTask.h"

class ThreadPool {
private:
    std::vector<std::thread> threads_;
    std::priority_queue<std::pair<int, size_t>> jobs_; // Пара: приоритет, индекс задачи
    std::mutex mutex_;
    std::condition_variable condition_;
    std::atomic<bool> stop_;
    int maxThreads_;
    std::atomic<size_t> activeTasks_;
    std::vector<FileTask>* tasksPtr_; // Указатель на tasks_ из Dispatcher

    void worker();

public:
    ThreadPool(int maxThreads);
    ~ThreadPool();
    void start(std::vector<FileTask>& tasks);
    void stop();
    bool isRunning() const;
    bool tasksCompleted() const;
};

#endif