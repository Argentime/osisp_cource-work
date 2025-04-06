#ifndef DISPATCHER_H
#define DISPATCHER_H

#include "FileTask.h"
#include "ExternalTool.h"
#include <vector>

class Dispatcher {
public:
    Dispatcher();
    ~Dispatcher() = default;

    // ���������� ��������
    void addTask(const FileTask& task);
    void runTasks();            // ��������� ������
    void clearTasks();          // ������� ������ �����

    // ����������
    size_t getTaskCount() const;
    size_t getCompletedCount() const;

    // ���������
    void setMaxThreads(int maxThreads); // ������������� �������� �������

private:
    std::vector<FileTask> tasks_;
    ExternalTool tool_;         // ����� ���������� ��� ���� �����
    int maxThreads_;            // ������������ ����� �������
};

#endif // DISPATCHER_H