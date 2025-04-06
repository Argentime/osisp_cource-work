#ifndef FILE_TASK_H
#define FILE_TASK_H

#include <string>

class ExternalTool; // Forward declaration

class FileTask {
public:
    FileTask(const std::string& filePath, const std::string& command, ExternalTool* tool);
    ~FileTask() = default;

    // Геттеры
    std::string getFilePath() const;
    std::string getCommand() const;
    bool isCompleted() const;
    int getResult() const;

    // Управление задачей
    void execute();         // Выполняет задачу
    void reset();           // Сбрасывает состояние
    void setPriority(int priority); // Устанавливает приоритет
    int getPriority() const;

private:
    std::string filePath_;
    std::string command_;
    ExternalTool* tool_;    // Указатель на инструмент
    bool completed_;
    int result_;            // Код возврата команды
    int priority_;          // Приоритет задачи
};

#endif // FILE_TASK_H