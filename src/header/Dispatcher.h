#ifndef DISPATCHER_H
#define DISPATCHER_H

#include "FileTask.h"
#include "ExternalTool.h"
#include "ThreadPool.h"
#include <vector>
#include <string>
#include <memory>

class Dispatcher {
public:
    Dispatcher();
    ~Dispatcher();

    void addTask(FileTask&& task);
    void runTasks();
    void clearTasks();
    size_t getTaskCount() const;
    size_t getCompletedCount() const;
    void setMaxThreads(int maxThreads);
    std::string getTaskList() const;
    std::string getSummary() const;
    bool tasksCompleted() const;

private:
    std::vector<FileTask> tasks_;
    ExternalTool tool_;
    int maxThreads_;
    std::unique_ptr<ThreadPool> threadPool_;
    bool isThreadPoolActive() const; // Новый метод
};

#endif // DISPATCHER_H