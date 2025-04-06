#ifndef EXTERNAL_TOOL_H
#define EXTERNAL_TOOL_H

#include <string>

class ExternalTool {
public:
    ExternalTool();
    ~ExternalTool() = default;

    void run(const std::string& command) const; // Запускает команду
    std::string getLastError() const;           // Возвращает последнюю ошибку
    bool isAvailable() const;                   // Проверяет доступность инструмента

private:
    mutable std::string lastError_; // Последняя ошибка (mutable для const методов)
};

#endif // EXTERNAL_TOOL_H