#ifndef CONSOLEINTERFACE_H
#define CONSOLEINTERFACE_H

#include "Dispatcher.h"
#include <string>

class ConsoleInterface {
private:
    Dispatcher& dispatcher_;
    std::vector<std::string> history_;
    size_t historyIndex_;
    std::string currentInput_;
    size_t cursorPos_;
    void showHelp() const;
    void processCommand(const std::string& command);
    void handleInput();
    void moveCursorLeft();
    void moveCursorRight();
    void moveCursorHome();
    void moveCursorEnd();
    void deleteChar();
    void backspace();
    void complete();
    std::vector<std::string> getCompletions(const std::string& text, bool isCommand);

public:
    ConsoleInterface(Dispatcher& dispatcher);
    void run();
};

#endif