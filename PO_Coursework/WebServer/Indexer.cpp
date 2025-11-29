// Indexer.cpp
#include "Indexer.h"
#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

Indexer::Indexer(InvertedIndex& idx, ThreadPool& tp, const std::string& path)
    : index(idx), pool(tp), directoryPath(path) {
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
}

void Indexer::run() {
    try {
        std::cout << "Scanning directory: " << directoryPath << std::endl;
        for (const auto& entry : fs::directory_iterator(directoryPath)) {
            if (entry.is_regular_file()) {
                // Копіюємо рядки, щоб передати їх у лямбду
                std::string path = entry.path().string();
                std::string name = entry.path().filename().string();

                // Додаємо задачу в пул
                pool.enqueue([this, path, name]() {
                    this->processFile(path, name);
                    });
            }
        }
        std::cout << "All indexing tasks submitted to ThreadPool.\n";
    }
    catch (const std::exception& e) {
        std::cerr << "Indexer Error: " << e.what() << std::endl;
    }
}