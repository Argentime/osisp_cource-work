#ifndef DISPATCHER_H
#define DISPATCHER_H

#include "FileTask.h"
#include "ExternalTool.h"
#include <vector>

class Dispatcher {
public:
    Dispatcher();
    ~Dispatcher() = default;

    // Управление задачами
    void addTask(const FileTask& task);
    void runTasks();            // Запускает задачи
    void clearTasks();          // Очищает список задач

    // Информация
    size_t getTaskCount() const;
    size_t getCompletedCount() const;

    // Настройки
    void setMaxThreads(int maxThreads); // Устанавливает максимум потоков

private:
    std::vector<FileTask> tasks_;
    ExternalTool tool_;         // Общий инструмент для всех задач
    int maxThreads_;            // Максимальное число потоков
};

#endif // DISPATCHER_H