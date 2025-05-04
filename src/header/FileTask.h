#ifndef FILE_TASK_H
#define FILE_TASK_H

#include <string>
#include <vector>
#include <mutex>
#include <atomic>

class ExternalTool;

class FileTask {
public:
    FileTask(const std::string& filePath, const std::vector<std::string>& params, ExternalTool* tool);
    ~FileTask() = default;

    // Запрещаем копирование
    FileTask(const FileTask&) = delete;
    FileTask& operator=(const FileTask&) = delete;

    // Разрешаем перемещение
    FileTask(FileTask&&) noexcept;
    FileTask& operator=(FileTask&&) noexcept;

    std::string getFilePath() const;
    std::vector<std::string> getParams() const;
    bool isCompleted() const;
    int getResult() const;
    void execute();
    void reset();
    void setPriority(int priority);
    int getPriority() const;

private:
    std::string filePath_;
    std::vector<std::string> params_;
    ExternalTool* tool_;
    std::atomic<bool> completed_; // Для потокобезопасного состояния
    std::atomic<int> result_;    // Для потокобезопасного результата
    std::atomic<int> priority_;
    mutable std::mutex mutex_;
};

#endif // FILE_TASK_H