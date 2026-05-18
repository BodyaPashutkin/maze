#pragma once
#include <string>
#include <vector>
#include <cstdint>

/**
 * @brief Состояния стен и посещения для каждой клетки лабиринта.
 * Используются битовые маски для экономии памяти.
 */
enum CellState : uint8_t {
    WALL_TOP    = 1 << 0, ///< Верхняя стена (00000001)
    WALL_RIGHT  = 1 << 1, ///< Правая стена (00000010)
    WALL_BOTTOM = 1 << 2, ///< Нижняя стена (00000100)
    WALL_LEFT   = 1 << 3, ///< Левая стена (00001000)
    VISITED     = 1 << 4  ///< Флаг посещения клетки (00010000)
};

/**
 * @class MazeGenerator
 * @brief Класс для генерации лабиринтов и поиска путей.
 *
 * Реализует генерацию идеального лабиринта с помощью алгоритма DFS с бэктрекингом,
 * а также поиск кратчайшего пути с помощью BFS. Вся память управляется автоматически (std::vector).
 */
class MazeGenerator {
private:
    int width;  ///< Ширина лабиринта в клетках
    int height; ///< Высота лабиринта в клетках
    std::vector<uint8_t> grid; ///< Одномерный массив, хранящий состояния всех клеток

    /**
     * @brief Переводит 2D координаты в 1D индекс массива.
     * @param x Координата X.
     * @param y Координата Y.
     * @return Индекс в массиве grid.
     */
    inline int getIndex(int x, int y) const;

    /**
     * @brief Проверяет, находится ли клетка в пределах поля и не посещена ли она.
     * @param x Координата X.
     * @param y Координата Y.
     * @return true, если клетка валидна и не посещена.
     */
    bool isValid(int x, int y) const;

public:
    /**
     * @brief Конструктор генератора. Инициализирует сетку со всеми стенами.
     * @param w Ширина лабиринта.
     * @param h Высота лабиринта.
     */
    MazeGenerator(int w, int h);

    /**
     * @brief Генерирует лабиринт алгоритмом DFS.
     * @param startX Стартовая X координата генерации (по умолчанию 0).
     * @param startY Стартовая Y координата генерации (по умолчанию 0).
     */
    void generate(int startX = 0, int startY = 0);

    /**
     * @brief Ищет кратчайший путь с помощью алгоритма поиска в ширину (BFS).
     * @param startX X старта.
     * @param startY Y старта.
     * @param endX X финиша (по умолчанию правый нижний угол).
     * @param endY Y финиша (по умолчанию правый нижний угол).
     * @return Вектор индексов клеток, составляющих путь.
     */
    std::vector<int> solveBFS(int startX = 0, int startY = 0, int endX = -1, int endY = -1);

    /**
     * @brief Выводит лабиринт в консоль (ASCII-графика).
     * @param playerX Координата X игрока (если -1, игрок не рисуется).
     * @param playerY Координата Y игрока (если -1, игрок не рисуется).
     * @param path Подсвечиваемый путь (по умолчанию пуст).
     */
    void printToConsole(int playerX = -1, int playerY = -1, const std::vector<int>& path = {}) const;

    /**
     * @brief Возвращает битовую маску стен для указанной клетки.
     * @param x Координата X.
     * @param y Координата Y.
     * @return Байт состояния (CellState).
     */
    uint8_t getCellWallState(int x, int y) const;

    // Геттеры для ширины и высоты
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    /**
     * @brief Сохраняет текущий лабиринт в текстовый файл.
     * @param filepath Путь к файлу для сохранения.
     * @throw std::runtime_error В случае ошибки открытия файла.
     */
    void saveToFile(const std::string& filepath) const;

    /**
     * @brief Загружает лабиринт из текстового файла.
     * @param filepath Путь к файлу для загрузки.
     * @throw std::runtime_error В случае ошибки чтения или повреждения данных.
     */
    void loadFromFile(const std::string& filepath);
};