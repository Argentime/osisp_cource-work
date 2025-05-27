cmake_minimum_required(VERSION 3.10)
project(FileDispatcher)
set(CMAKE_CXX_STANDARD 17)
add_compile_options(-g -ggdb -std=c11 -pedantic -W -Wall -Wextra)
include_directories(header)
add_executable(dispatcher
    main.cpp
    FileTask.cpp
    ExternalTool.cpp
    Dispatcher.cpp
    ThreadPool.cpp
    ConsoleInterface.cpp
    BatchParser.cpp
)
find_package(Threads REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(dispatcher PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(dispatcher PRIVATE Threads::Threads)