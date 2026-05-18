#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "MazeGenerator.h"
#include <stdexcept>
#include <fstream>

TEST_CASE("Тестирование базовой генерации и размеров") {
    MazeGenerator maze(10, 15);
    
    // Положительный тест: проверяем геттеры
    CHECK(maze.getWidth() == 10);
    CHECK(maze.getHeight() == 15);
}

TEST_CASE("Тестирование поиска пути (BFS)") {
    MazeGenerator maze(5, 5);
    maze.generate();
    
    // Положительный тест: путь из начала в конец должен существовать в идеальном лабиринте
    std::vector<int> path = maze.solveBFS(0, 0, 4, 4);
    CHECK(path.empty() == false);
    CHECK(path.front() == 0); // Индекс старта (0*5+0)
    CHECK(path.back() == 24); // Индекс финиша (4*5+4)

    // Негативный/Граничный тест: путь в ту же самую клетку
    std::vector<int> zeroPath = maze.solveBFS(2, 2, 2, 2);
    CHECK(zeroPath.size() == 1);
}

TEST_CASE("Тестирование файлового ввода/вывода и исключений") {
    MazeGenerator maze(3, 3);
    maze.generate();
    std::string testFile = "test_maze.txt";

    // Положительный тест: сохранение и загрузка
    REQUIRE_NOTHROW(maze.saveToFile(testFile)); // Не должно быть исключений
    
    MazeGenerator loadedMaze(1, 1);
    REQUIRE_NOTHROW(loadedMaze.loadFromFile(testFile));
    
    CHECK(loadedMaze.getWidth() == 3);
    CHECK(loadedMaze.getHeight() == 3);

    // Негативный тест: попытка загрузить несуществующий файл должна выбросить исключение
    CHECK_THROWS_AS(maze.loadFromFile("invalid_path_that_does_not_exist.txt"), std::runtime_error);
    
    // Негативный тест: попытка загрузить сломанный файл
    std::ofstream badFile("bad_maze.txt");
    badFile << "not a number";
    badFile.close();
    CHECK_THROWS_AS(maze.loadFromFile("bad_maze.txt"), std::runtime_error);
}