#include "Dispatcher.h"
#include <iostream>

int main() {
    Dispatcher dispatcher;
    ExternalTool tool;

    // Пример задач
    FileTask task1("file1.txt", "ls -l file1.txt", &tool);
    FileTask task2("file2.txt", "ls -l file2.txt", &tool);

    dispatcher.addTask(task1);
    dispatcher.addTask(task2);

    std::cout << "Total tasks: " << dispatcher.getTaskCount() << "\n";
    dispatcher.setMaxThreads(4); // Пока заглушка
    dispatcher.runTasks();

    std::cout << "Completed tasks: " << dispatcher.getCompletedCount() << "\n";

    return 0;
}