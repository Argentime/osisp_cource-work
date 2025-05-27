#ifndef BATCHPARSER_H
#define BATCHPARSER_H

#include "FileTask.h"
#include <vector>
#include <string>

class BatchParser {
public:
    std::vector<FileTask> loadTasks(const std::string& filename, ExternalTool* tool);
};

#endif