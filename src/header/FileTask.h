#ifndef FILE_TASK_H
#define FILE_TASK_H

#include <string>

class ExternalTool; // Forward declaration

class FileTask {
public:
    FileTask(const std::string& filePath, const std::string& command, ExternalTool* tool);
    ~FileTask() = default;

    // �������
    std::string getFilePath() const;
    std::string getCommand() const;
    bool isCompleted() const;
    int getResult() const;

    // ���������� �������
    void execute();         // ��������� ������
    void reset();           // ���������� ���������
    void setPriority(int priority); // ������������� ���������
    int getPriority() const;

private:
    std::string filePath_;
    std::string command_;
    ExternalTool* tool_;    // ��������� �� ����������
    bool completed_;
    int result_;            // ��� �������� �������
    int priority_;          // ��������� ������
};

#endif // FILE_TASK_H