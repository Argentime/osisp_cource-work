#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include "FileTask.h"
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>

class ThreadPool {
public:
    ThreadPool(int maxThreads);
    ~ThreadPool();
    void start(std::vector<FileTask>& tasks);
    void stop();
    bool isRunning() const;
    bool tasksCompleted() const;

private:
    std::vector<std::thread> threads_;
    std::queue<std::function<void()>> jobs_;
    std::mutex mutex_;
    std::condition_variable condition_;
    std::atomic<bool> stop_;
    int maxThreads_;
    std::atomic<size_t> activeTasks_;
    void worker();
};

#endif // THREAD_POOL_H