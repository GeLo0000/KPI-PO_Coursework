// Indexer.cpp
#include "Indexer.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <chrono> // Для сну (sleep)

namespace fs = std::filesystem;

Indexer::Indexer(InvertedIndex& idx, ThreadPool& tp, const std::string& path)
    : index(idx), pool(tp), directoryPath(path), running(false) {
}

Indexer::~Indexer() {
    stop();
}

void Indexer::start() {
    if (running) return; // Вже запущено
    running = true;

    // Запускаємо indexingLoop в окремому потоці
    schedulerThread = std::thread(&Indexer::indexingLoop, this);

    std::cout << "[SCHEDULER] Started monitoring directory: " << directoryPath << "\n";
}

void Indexer::stop() {
    running = false;
    if (schedulerThread.joinable()) {
        schedulerThread.join();
    }
}

void Indexer::indexingLoop() {
    while (running) {
        try {
            bool foundNew = false;

            // Проходимось по папці
            for (const auto& entry : fs::directory_iterator(directoryPath)) {
                if (entry.is_regular_file()) {
                    std::string filename = entry.path().filename().string();

                    // Перевіряємо, чи бачили ми вже цей файл
                    // count повертає 1 якщо є, 0 якщо немає
                    if (seenFiles.count(filename) == 0) {

                        // НОВИЙ ФАЙЛ!
                        // 1. Запам'ятовуємо його
                        seenFiles.insert(filename);

                        // 2. Відправляємо в пул
                        std::string path = entry.path().string();
                        pool.enqueue([this, path, filename]() {
                            this->processFile(path, filename);
                            });

                        std::cout << "[SCHEDULER] Found new file: " << filename << "\n";
                        foundNew = true;
                    }
                }
            }

            if (foundNew) {
                std::cout << "[SCHEDULER] New tasks submitted to ThreadPool.\n";
            }

        }
        catch (const std::exception& e) {
            std::cerr << "Indexer Error: " << e.what() << std::endl;
        }

        // Спимо 5 секунд перед наступною перевіркою
        // (використовуємо цикл з коротким сном, щоб швидше зреагувати на stop())
        for (int i = 0; i < 50; ++i) {
            if (!running) break;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}

void Indexer::processFile(const std::string& filepath, const std::string& filename) {
    std::ifstream file(filepath);
    if (!file.is_open()) return;

    std::string word;
    while (file >> word) {
        std::string processed = InvertedIndex::cleanWord(word);
        if (!processed.empty()) {
            index.add(processed, filename);
        }
    }
    index.incrementFileCount();
}