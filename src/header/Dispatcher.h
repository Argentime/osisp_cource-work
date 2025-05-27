#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <vector>
#include <memory>
#include "FileTask.h"
#include "ExternalTool.h"
#include "ThreadPool.h"

class Dispatcher {
private:
    std::vector<FileTask> tasks_;
    ExternalTool tool_;
    int maxThreads_;
    std::unique_ptr<ThreadPool> threadPool_;
    bool isThreadPoolActive() const;

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
    ExternalTool* getTool(); // Новый метод для доступа к tool_
};

#endif