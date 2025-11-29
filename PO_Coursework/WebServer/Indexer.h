// Indexer.h
#pragma once
#include <string>
#include <vector>
#include "InvertedIndex.h"
#include "ThreadPool.h"

class Indexer {
private:
    InvertedIndex& index; // Посилання на індекс, який ми наповнюємо
    ThreadPool& pool;     // Посилання на пул, який ми використовуємо
    std::string directoryPath;

    // Приватний метод для обробки одного файлу
    void processFile(const std::string& filepath, const std::string& filename);

public:
    Indexer(InvertedIndex& idx, ThreadPool& tp, const std::string& path);

    // Запускає індексацію
    void run();
};