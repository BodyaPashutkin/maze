cmake_minimum_required(VERSION 3.30)
project(maze)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Собираем ядро лабиринта как статическую библиотеку
add_library(MazeLib MazeGenerator.cpp)

# 2. Собираем основное интерактивное приложение
add_executable(maze_app main.cpp)
target_link_libraries(maze_app PRIVATE MazeLib)

# 3. Интеграция фреймворка Doctest
include(FetchContent)
FetchContent_Declare(
        doctest
        GIT_REPOSITORY https://github.com/doctest/doctest.git
        GIT_TAG v2.4.11
)
FetchContent_MakeAvailable(doctest)

# 4. Настройка тестов
enable_testing()
add_executable(maze_tests tests.cpp)
target_link_libraries(maze_tests PRIVATE MazeLib doctest::doctest)

# Регистрируем тесты для команды ctest
add_test(NAME MazeTests COMMAND maze_tests)