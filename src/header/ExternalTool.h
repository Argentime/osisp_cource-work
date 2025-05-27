#ifndef EXTERNALTOOL_H
#define EXTERNALTOOL_H

#include <string>

class ExternalTool {
private:
    std::string lastError_;
    bool isCommandSafe(const std::string& command) const;

public:
    ExternalTool();
    void run(std::string command); // ������ const
    std::string runWithOutput(std::string command); // ������ const
    std::string getLastError() const;
    bool isAvailable() const;
};

#endif