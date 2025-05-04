#include "ConsoleInterface.h"
#include <iostream>
#include <sstream>
#include <vector>

ConsoleInterface::ConsoleInterface(Dispatcher& dispatcher) : dispatcher_(dispatcher) {}

void ConsoleInterface::run() {
    std::string command;
    std::cout << "File Dispatcher Console. Type 'help' for commands.\n";
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, command);
        if (command == "exit") break;
        processCommand(command);
    }
}

void ConsoleInterface::showHelp() const {
    std::cout << "Available commands:\n"
        << "  add <file> <param1> <param2> ... [priority <number>] - Add a task with parameters (-a, -b, etc.)\n"
        << "  list - Show all tasks\n"
        << "  run - Run all tasks\n"
        << "  clear - Clear all tasks\n"
        << "  threads <number> - Set maximum number of threads\n"
        << "  summary - Show execution summary\n"
        << "  help - Show this help\n"
        << "  exit - Exit the program\n"
        << "Supported parameters: -a (grep), -b (ls -l), -c (wc -l)\n";
}

void ConsoleInterface::processCommand(const std::string& command) {
    std::istringstream iss(command);
    std::vector<std::string> tokens;
    std::string token;
    while (iss >> token) tokens.push_back(token);

    if (tokens.empty()) return;

    if (tokens[0] == "help") {
        showHelp();
    }
    else if (tokens[0] == "add" && tokens.size() >= 3) {
        std::string filePath = tokens[1];
        std::vector<std::string> params;
        size_t i = 2;
        while (i < tokens.size() && tokens[i] != "priority") {
            if (tokens[i].substr(0, 1) == "-") {
                params.push_back(tokens[i]);
            }
            else {
                std::cout << "Invalid parameter: " << tokens[i] << ". Must start with '-'\n";
                return;
            }
            ++i;
        }
        int priority = 0;
        if (i + 1 < tokens.size() && tokens[i] == "priority") {
            try {
                priority = std::stoi(tokens[i + 1]);
            }
            catch (...) {
                std::cout << "Invalid priority\n";
                return;
            }
        }
        ExternalTool tool;
        FileTask task(filePath, params, &tool);
        task.setPriority(priority);
        dispatcher_.addTask(std::move(task)); // Перемещение задачи
        std::cout << "Task added: " << filePath << "\n";
    }
    else if (tokens[0] == "list") {
        std::cout << dispatcher_.getTaskList();
    }
    else if (tokens[0] == "run") {
        dispatcher_.runTasks();
    }
    else if (tokens[0] == "clear") {
        dispatcher_.clearTasks();
        std::cout << "All tasks cleared\n";
    }
    else if (tokens[0] == "threads" && tokens.size() == 2) {
        try {
            int threads = std::stoi(tokens[1]);
            dispatcher_.setMaxThreads(threads);
        }
        catch (...) {
            std::cout << "Invalid number of threads\n";
        }
    }
    else if (tokens[0] == "summary") {
        std::cout << dispatcher_.getSummary();
    }
    else {
        std::cout << "Unknown command. Type 'help' for list.\n";
    }
}