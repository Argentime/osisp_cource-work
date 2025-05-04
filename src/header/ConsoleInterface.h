#ifndef CONSOLE_INTERFACE_H
#define CONSOLE_INTERFACE_H

#include "Dispatcher.h"
#include <string>

class ConsoleInterface {
public:
    ConsoleInterface(Dispatcher& dispatcher);
    void run();

private:
    Dispatcher& dispatcher_;
    void showHelp() const;
    void processCommand(const std::string& command);
};

#endif // CONSOLE_INTERFACE_H