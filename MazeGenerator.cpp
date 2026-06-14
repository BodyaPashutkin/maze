#include "MazeGenerator.h"
#include <iostream>
#include <random>
#include <queue>
#include <fstream>
#include <stdexcept>
#include <string>

inline int MazeGenerator::getIndex(int x, int y) const {
    return y * width + x;
};

bool MazeGenerator::isValid(int x, int y) const {
    if (x < 0 || x >= width || y < 0 || y >= height) return false;
    return !(grid[getIndex(x, y)] & VISITED);
};

MazeGenerator::MazeGenerator(int w, int h) : width(w), height(h) {
    uint8_t initial_state = WALL_TOP | WALL_RIGHT | WALL_BOTTOM | WALL_LEFT;
    grid.assign(width * height, initial_state);
};

void MazeGenerator::generate(int startX, int startY) {
    // Инициализируем генератор случайных чисел
    std::random_device rd;
    std::mt19937 rng(rd());

    // Наш явный стек, хранящий координаты {x, y}
    std::vector<std::pair<int, int>> stack;

    // Отмечаем стартовую клетку как посещенную и кладем в стек
    grid[getIndex(startX, startY)] |= VISITED;
    stack.push_back({startX, startY});

    // Удобная структура для перебора направлений
    // (смещение x, смещение y, стена со стороны текущей, стена со стороны соседа)
    struct Direction { int dx, dy; CellState wall, opp_wall; };
    Direction dirs[4] = {
        {0, -1, WALL_TOP, WALL_BOTTOM},  // Вверх
        {1,  0, WALL_RIGHT, WALL_LEFT},  // Вправо
        {0,  1, WALL_BOTTOM, WALL_TOP},  // Вниз
        {-1, 0, WALL_LEFT, WALL_RIGHT}   // Влево
    };

    while (!stack.empty()) {
        // Берем клетку на вершине стека (без удаления)
        int cx = stack.back().first;
        int cy = stack.back().second;

        // Ищем всех непосещенных соседей
        std::vector<Direction> unvisited_neighbors;
        unvisited_neighbors.reserve(4); // Небольшая оптимизация аллокации

        for (const auto& dir : dirs) {
            int nx = cx + dir.dx;
            int ny = cy + dir.dy;
            if (isValid(nx, ny)) {
                unvisited_neighbors.push_back(dir);
            }
        }

        if (!unvisited_neighbors.empty()) {
            // Выбираем случайного соседа
            std::uniform_int_distribution<int> dist(0, unvisited_neighbors.size() - 1);
            Direction chosen = unvisited_neighbors[dist(rng)];

            int nx = cx + chosen.dx;
            int ny = cy + chosen.dy;

            // Сносим стены между текущей клеткой и выбранным соседом
            // Побитовое И с инвертированной маской (&= ~) сбрасывает нужный бит в 0
            grid[getIndex(cx, cy)] &= ~chosen.wall;
            grid[getIndex(nx, ny)] &= ~chosen.opp_wall;

            // Отмечаем соседа как посещенного и отправляем его в стек (шаг вперед)
            grid[getIndex(nx, ny)] |= VISITED;
            stack.push_back({nx, ny});
        } else {
            // Тупик. Бэктрекинг — снимаем клетку со стека (шаг назад)
            stack.pop_back();
        }
    }
};

std::vector<int> MazeGenerator::solveBFS(int startX, int startY, int endX, int endY) {
  // Ищет путь от (startX, startY) до (endX, endY). По умолчанию из левого верхнего в правый нижний угол.
    if (endX == -1) endX = width - 1;
    if (endY == -1) endY = height - 1;

    int startIndex = getIndex(startX, startY);
    int endIndex = getIndex(endX, endY);

    std::queue<int> q;
    std::vector<bool> visited(width * height, false);

    // Массив для восстановления пути. Изначально заполнен -1 (нет родителя)
    std::vector<int> parent(width * height, -1);

    // Начинаем со стартовой клетки
    q.push(startIndex);
    visited[startIndex] = true;

    while (!q.empty()) {
        int curr = q.front();
        q.pop();

        if (curr == endIndex) break; // Дошли до финиша, прекращаем поиск

        int cx = curr % width;
        int cy = curr / width;
        uint8_t cell = grid[curr];

        // Проверяем все 4 направления.
        // Мы можем пойти туда, если: 1) нет стены, 2) мы там еще не были

        // Вверх
        if (!(cell & WALL_TOP) && !visited[getIndex(cx, cy - 1)]) {
            int next = getIndex(cx, cy - 1);
            visited[next] = true; parent[next] = curr; q.push(next);
        }
        // Вправо
        if (!(cell & WALL_RIGHT) && !visited[getIndex(cx + 1, cy)]) {
            int next = getIndex(cx + 1, cy);
            visited[next] = true; parent[next] = curr; q.push(next);
        }
        // Вниз
        if (!(cell & WALL_BOTTOM) && !visited[getIndex(cx, cy + 1)]) {
            int next = getIndex(cx, cy + 1);
            visited[next] = true; parent[next] = curr; q.push(next);
        }
        // Влево
        if (!(cell & WALL_LEFT) && !visited[getIndex(cx - 1, cy)]) {
            int next = getIndex(cx - 1, cy);
            visited[next] = true; parent[next] = curr; q.push(next);
        }
    }

    // Восстанавливаем путь, раскручивая массив parent с финиша до старта
    std::vector<int> path;
    for (int at = endIndex; at != -1; at = parent[at]) {
        path.push_back(at);
    }

    // Так как мы шли с конца, переворачиваем массив, чтобы путь был от старта к финишу
    std::reverse(path.begin(), path.end());

    // Если до финиша нельзя дойти, возвращаем пустой вектор
    if (path.size() == 1 && startIndex != endIndex) return {};

    return path;
};

void MazeGenerator::printToConsole(int playerX, int playerY, const std::vector<int>& path) const {
// Добавляем аргумент со значением по умолчанию (пустой путь)
    // Создаем быстрый массив-маску для проверки, принадлежит ли клетка пути
    std::vector<bool> isPath(width * height, false);
    for (int index : path) {
        isPath[index] = true;
    }

    for (int x = 0; x < width; ++x) std::cout << "+---";
    std::cout << "+\n";

    for (int y = 0; y < height; ++y) {
        std::cout << "|";
        for (int x = 0; x < width; ++x) {
            uint8_t cell = grid[getIndex(x, y)];

            if (x == playerX && y == playerY) {
                std::cout << " @ "; // Игрок
            } else if (x == width - 1 && y == height - 1) {
                std::cout << " F "; // Финиш в правом нижнем углу
            } else if (isPath[getIndex(x, y)]) {
                std::cout << " * "; // Путь-подсказка
            } else {
                std::cout << "   "; // Пустота
            }
            if (cell & WALL_RIGHT) std::cout << "|";
            else                   std::cout << " ";
        }
        std::cout << "\n";

        std::cout << "+";
        for (int x = 0; x < width; ++x) {
            uint8_t cell = grid[getIndex(x, y)];
            if (cell & WALL_BOTTOM) std::cout << "---";
            else                    std::cout << "   ";
            std::cout << "+";
        }
        std::cout << "\n";
    }
};

uint8_t MazeGenerator::getCellWallState(int x, int y) const {
    return grid[getIndex(x, y)];
};

void MazeGenerator::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Критическая ошибка: не удалось открыть файл для записи по пути " + filepath);
    }

    // Пишем размеры лабиринта
    file << width << " " << height << "\n";

    // Пишем сырые данные (состояния клеток)
    for (uint8_t cell : grid) {
        file << static_cast<int>(cell) << " ";
    }

    file.close();
};

void MazeGenerator::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Критическая ошибка: не удалось открыть файл для чтения по пути " + filepath);
    }

    int w, h;
    if (!(file >> w >> h)) {
        throw std::runtime_error("Ошибка формата файла: невозможно прочитать размеры лабиринта из " + filepath);
    }

    width = w;
    height = h;
    grid.clear();
    grid.reserve(width * height);

    int cellData;
    while (file >> cellData) {
        grid.push_back(static_cast<uint8_t>(cellData));
    }

    if (grid.size() != static_cast<size_t>(width * height)) {
        throw std::runtime_error("Повреждение данных: количество клеток в файле не соответствует заявленным размерам.");
    }

    file.close();
};