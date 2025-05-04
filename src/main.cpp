#include "Dispatcher.h"
#include "ConsoleInterface.h"

int main() {
    Dispatcher dispatcher;
    ConsoleInterface console(dispatcher);
    console.run();
    return 0;
}