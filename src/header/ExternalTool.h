#ifndef EXTERNAL_TOOL_H
#define EXTERNAL_TOOL_H

#include <string>

class ExternalTool {
public:
    ExternalTool();
    ~ExternalTool() = default;

    void run(const std::string& command) const;
    std::string getLastError() const;
    bool isAvailable() const;

private:
    mutable std::string lastError_;
};

#endif // EXTERNAL_TOOL_H