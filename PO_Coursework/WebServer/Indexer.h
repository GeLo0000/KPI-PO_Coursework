// Indexer.h
#pragma once
#include <string>
#include <vector>
#include <set>           // <--- Додано
#include <thread>        // <--- Додано
#include <atomic>        // <--- Додано
#include "InvertedIndex.h"
#include "ThreadPool.h"

class Indexer {
private:
    InvertedIndex& index;
    ThreadPool& pool;
    std::string directoryPath;

    // "Пам'ять" планувальника: список файлів, які ми вже бачили
    std::set<std::string> seenFiles;

    // Потік для фонового сканування
    std::thread schedulerThread;
    std::atomic<bool> running;

    void processFile(const std::string& filepath, const std::string& filename);

    // Основний цикл, який буде крутитися вічно
    void indexingLoop();

public:
    Indexer(InvertedIndex& idx, ThreadPool& tp, const std::string& path);
    ~Indexer(); // Деструктор для зупинки потоку

    // Метод запуску (тепер він не блокує main, а просто запускає потік)
    void start();

    // Метод зупинки
    void stop();
};