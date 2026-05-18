#include "MazeGenerator.h"
#include <iostream>
#include <vector>
#include <cstdint>
#include <random>
#include <unistd.h>
#include <termios.h>

// Функция для чтения одного символа без ожидания Enter
char getch() {
    char buf = 0;
    struct termios old = {0};
    if (tcgetattr(0, &old) < 0) perror("tcsetattr()");
    old.c_lflag &= ~ICANON; // Отключаем канонический режим
    old.c_lflag &= ~ECHO;   // Отключаем эхо (чтобы вводимые буквы не печатались)
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &old) < 0) perror("tcsetattr ICANON");
    if (read(0, &buf, 1) < 0) perror ("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old) < 0) perror ("tcsetattr ~ICANON");
    return buf;
};

// Функция для очистки экрана консоли (ANSI escape-последовательность)
void clearScreen() {
    std::cout << "\033[2J\033[1;1H";
};

int main(int argc, char* argv[]) {
    // Парсинг аргументов командной строки
    std::string loadPath = "";
    std::string savePath = "";
    int width = 20;
    int height = 10;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--load" && i + 1 < argc) {
            loadPath = argv[++i];
        } else if (arg == "--save" && i + 1 < argc) {
            savePath = argv[++i];
        }
    }

    try {
        MazeGenerator maze(width, height);

        if (!loadPath.empty()) {
            std::cout << "Загрузка лабиринта из файла: " << loadPath << "...\n";
            maze.loadFromFile(loadPath);
            width = maze.getWidth();
            height = maze.getHeight();
        } else {
            maze.generate();
            if (!savePath.empty()) {
                std::cout << "Сохранение сгенерированного лабиринта в файл: " << savePath << "...\n";
                maze.saveToFile(savePath);
            }
        }

        // Интерактивный цикл (остается без изменений)
        int playerX = 0;
        int playerY = 0;
        std::vector<int> currentPath;
        bool showHint = false;

        while (true) {
            clearScreen();

            if (showHint) {
                currentPath = maze.solveBFS(playerX, playerY);
                maze.printToConsole(playerX, playerY, currentPath);
                showHint = false;
            } else {
                maze.printToConsole(playerX, playerY);
            }

            std::cout << "\nУправление: W/A/S/D. Подсказка: H. Выход: Q.\n";

            if (playerX == width - 1 && playerY == height - 1) {
                std::cout << "\nПОЗДРАВЛЯЮ! ВЫ ПРОШЛИ ЛАБИРИНТ!\n";
                break;
            }

            char input = getch();
            uint8_t cell = maze.getCellWallState(playerX, playerY);

            if      ((input == 'w' || input == 'W') && !(cell & WALL_TOP))    playerY--;
            else if ((input == 's' || input == 'S') && !(cell & WALL_BOTTOM)) playerY++;
            else if ((input == 'a' || input == 'A') && !(cell & WALL_LEFT))   playerX--;
            else if ((input == 'd' || input == 'D') && !(cell & WALL_RIGHT))  playerX++;
            else if (input == 'h' || input == 'H') showHint = true;
            else if (input == 'q' || input == 'Q') break;
        }

    } catch (const std::exception& e) {
        // Перехватываем все исключения (ошибки файлов, повреждения памяти и т.д.)
        std::cerr << "\n[ОШИБКА ПРИЛОЖЕНИЯ]: " << e.what() << "\n";
        return 1; // Возвращаем ненулевой код ошибки
    }

    return 0;
}