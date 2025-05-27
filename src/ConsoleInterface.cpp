#include "ConsoleInterface.h"
#include "BatchParser.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <termios.h>
#include <unistd.h>
#include <dirent.h>
#include <algorithm>
#include <cstring>

ConsoleInterface::ConsoleInterface(Dispatcher& dispatcher)
    : dispatcher_(dispatcher), historyIndex_(0), cursorPos_(0) {
    history_.push_back("");
}

void ConsoleInterface::showHelp() const {
    std::cout << "Commands:\n"
        << "  add <file> -cmd \"<command>\" [priority <n>] - Add task\n"
        << "  run - Run all tasks\n"
        << "  list - List all tasks\n"
        << "  summary - Show summary\n"
        << "  clear - Clear all tasks\n"
        << "  threads <n> - Set max threads\n"
        << "  batch <file.json> - Load tasks from JSON\n"
        << "  help - Show this help\n"
        << "  exit - Exit program\n";
}

std::vector<std::string> ConsoleInterface::getCompletions(const std::string& text, bool isCommand) {
    std::vector<std::string> completions;
    if (isCommand) {
        std::vector<std::string> commands = {
            "add", "run", "list", "summary", "clear", "threads", "batch", "help", "exit"
        };
        for (const auto& cmd : commands) {
            if (cmd.rfind(text, 0) == 0) completions.push_back(cmd);
        }
    }
    else {
        DIR* dir = opendir(".");
        if (dir) {
            struct dirent* entry;
            while ((entry = readdir(dir))) {
                std::string name = entry->d_name;
                if (name != "." && name != ".." && name.rfind(text, 0) == 0) {
                    if (text.rfind("batch ", 0) == 0 && name.size() >= 5 && name.substr(name.size() - 5) != ".json") {
                        continue; // Для batch только .json
                    }
                    completions.push_back(name);
                }
            }
            closedir(dir);
        }
    }
    return completions;
}

void ConsoleInterface::complete() {
    std::string text;
    bool isCommand = currentInput_.empty() || currentInput_.find(' ') == std::string::npos;
    if (isCommand) {
        text = currentInput_;
    }
    else {
        size_t lastSpace = std::string::npos;
        if (cursorPos_ > 0) {
            lastSpace = currentInput_.rfind(' ', cursorPos_ - 1);
        }
        text = (lastSpace == std::string::npos) ? currentInput_ : currentInput_.substr(lastSpace + 1);
    }

    auto completions = getCompletions(text, isCommand);
    if (completions.empty()) return;

    if (completions.size() == 1) {
        if (isCommand) {
            currentInput_ = completions[0] + " ";
        }
        else {
            size_t lastSpace = std::string::npos;
            if (cursorPos_ > 0) {
                lastSpace = currentInput_.rfind(' ', cursorPos_ - 1);
            }
            if (lastSpace == std::string::npos) {
                currentInput_ = completions[0];
            }
            else {
                currentInput_ = currentInput_.substr(0, lastSpace + 1) + completions[0];
            }
        }
        cursorPos_ = currentInput_.size();
        std::cout << "\033[2K\033[G> " << currentInput_ << "\033[" << (currentInput_.size() - cursorPos_) << "D";
    }
    else {
        std::cout << "\n";
        for (const auto& c : completions) std::cout << c << " ";
        std::cout << "\n> " << currentInput_;
        std::cout.flush();
    }
}

void ConsoleInterface::moveCursorLeft() {
    if (cursorPos_ > 0) {
        cursorPos_--;
        std::cout << "\033[D";
    }
}

void ConsoleInterface::moveCursorRight() {
    if (cursorPos_ < currentInput_.size()) {
        cursorPos_++;
        std::cout << "\033[C";
    }
}

void ConsoleInterface::moveCursorHome() {
    if (cursorPos_ > 0) {
        std::cout << "\033[" << cursorPos_ << "D";
        cursorPos_ = 0;
    }
}

void ConsoleInterface::moveCursorEnd() {
    if (cursorPos_ < currentInput_.size()) {
        std::cout << "\033[" << (currentInput_.size() - cursorPos_) << "C";
        cursorPos_ = currentInput_.size();
    }
}

void ConsoleInterface::backspace() {
    if (cursorPos_ > 0) {
        cursorPos_--;
        currentInput_.erase(cursorPos_, 1);
        std::cout << "\033[2K\r> " << currentInput_;
        int rightShift = currentInput_.size() - cursorPos_;
        if (rightShift > 0)
            std::cout << "\033[" << rightShift << "D";
    }
}


void ConsoleInterface::deleteChar() {
    if (cursorPos_ < currentInput_.size()) {
        currentInput_.erase(cursorPos_, 1);
        std::cout << "\033[K" << currentInput_.substr(cursorPos_) << "\033[" << (currentInput_.size() - cursorPos_) << "D";
    }
}

void ConsoleInterface::handleInput() {
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    currentInput_.clear();
    cursorPos_ = 0;
    std::cout << "> ";
    std::cout.flush();

    while (true) {
        char c;
        ssize_t n = read(STDIN_FILENO, &c, 1);
        if (n <= 0) break; // EOF or error

        if (c == '\n') {
            std::cout << std::endl;
            if (!currentInput_.empty()) {
                history_.back() = currentInput_;
                history_.push_back("");
                historyIndex_ = history_.size() - 1;
            }
            break;
        }
        else if (c == '\t') {
            complete();
        }
        else if (c == 127) {
            backspace();
        }
        else if (c == 27) {
            char seq[3] = { 0 };
            if (read(STDIN_FILENO, &seq[0], 1) != 1 || read(STDIN_FILENO, &seq[1], 1) != 1) continue;
            if (seq[0] == '[') {
                switch (seq[1]) {
                case 'A': // Up
                    if (historyIndex_ > 0) {
                        historyIndex_--;
                        currentInput_ = history_[historyIndex_];
                        cursorPos_ = currentInput_.size();
                        std::cout << "\033[2K\033[G> " << currentInput_;
                    }
                    break;
                case 'B': // Down
                    if (historyIndex_ < history_.size() - 1) {
                        historyIndex_++;
                        currentInput_ = history_[historyIndex_];
                        cursorPos_ = currentInput_.size();
                        std::cout << "\033[2K\033[G> " << currentInput_;
                    }
                    break;
                case 'C': moveCursorRight(); break;
                case 'D': moveCursorLeft(); break;
                case 'H': moveCursorHome(); break;
                case 'F': moveCursorEnd(); break;
                case '3': // Delete
                    if (read(STDIN_FILENO, &seq[2], 1) == 1 && seq[2] == '~') {
                        deleteChar();
                    }
                    break;
                }
            }
        }
        else if (c == 3) { // Ctrl+C
            std::cout << "^C\n> ";
            currentInput_.clear();
            cursorPos_ = 0;
            std::cout.flush();
        }
        else if (c >= 32 && c <= 126) { // Печатные символы
            currentInput_.insert(cursorPos_, 1, c);
            cursorPos_++;
            std::cout << "\033[2K\r> " << currentInput_;
            int rightShift = currentInput_.size() - cursorPos_;
            if (rightShift > 0)
                std::cout << "\033[" << rightShift << "D";
        }
        std::cout.flush();
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

void ConsoleInterface::processCommand(const std::string& command) {
    if (command.empty()) return;

    std::istringstream iss(command);
    std::string cmd;
    iss >> cmd;

    if (cmd == "add") {
        std::string file, commandStr, token;
        int priority = 0;
        bool cmdFlag = false;
        while (iss >> token) {
            if (token == "-cmd") {
                cmdFlag = true;
                std::getline(iss >> std::ws, commandStr);
                commandStr.erase(0, commandStr.find_first_not_of("\""));
                commandStr.erase(commandStr.find_last_not_of("\"") + 1);
            }
            else if (token == "priority") {
                iss >> priority;
            }
            else if (!file.empty()) {
                commandStr += " " + token;
            }
            else {
                file = token;
            }
        }
        if (!file.empty() && cmdFlag) {
            dispatcher_.addTask(FileTask(file, commandStr, dispatcher_.getTool(), priority));
        }
        else {
            std::cout << "Invalid add command. Use: add <file> -cmd \"<command>\" [priority <n>]\n";
        }
    }
    else if (cmd == "run") {
        dispatcher_.runTasks();
    }
    else if (cmd == "list") {
        std::cout << dispatcher_.getTaskList();
    }
    else if (cmd == "summary") {
        std::cout << dispatcher_.getSummary() << std::endl;
    }
    else if (cmd == "clear") {
        dispatcher_.clearTasks();
        std::cout << "Tasks cleared\n";
    }
    else if (cmd == "threads") {
        int n;
        if (iss >> n) {
            dispatcher_.setMaxThreads(n);
        }
        else {
            std::cout << "Invalid threads command. Use: threads <n>\n";
        }
    }
    else if (cmd == "batch") {
        std::string filename;
        if (iss >> filename) {
            BatchParser parser;
            auto tasks = parser.loadTasks(filename, dispatcher_.getTool());
            for (auto& task : tasks) {
                dispatcher_.addTask(std::move(task));
            }
            std::cout << "Loaded " << tasks.size() << " tasks from " << filename << std::endl;
        }
        else {
            std::cout << "Invalid batch command. Use: batch <file.json>\n";
        }
    }
    else if (cmd == "help") {
        showHelp();
    }
    else if (cmd == "exit") {
        return;
    }
    else {
        std::cout << "Unknown command. Type 'help' for commands.\n";
    }
}

void ConsoleInterface::run() {
    showHelp();
    while (true) {
        handleInput();
        if (!currentInput_.empty()) {
            processCommand(currentInput_);
            if (currentInput_ == "exit") break;
        }
    }
}