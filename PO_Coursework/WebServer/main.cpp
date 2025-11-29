// main.cpp
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <algorithm> // Для transform (lower case)

#include "InvertedIndex.h"

namespace fs = std::filesystem;

// Допоміжна функція для очистки слова (прибираємо коми, крапки, переводимо в нижній регістр)
std::string cleanWord(std::string word) {
    std::string clean;
    for (char c : word) {
        // Залишаємо тільки букви та цифри
        if (isalnum((unsigned char)c)) {
            clean += tolower((unsigned char)c);
        }
    }
    return clean;
}

int main() {
    // 1. Ініціалізація
    InvertedIndex idx;
    std::string path = "data"; // Назва вашої папки

    std::cout << "Indexing files from: " << path << "...\n";

    // 2. Сканування папки та індексація
    try {
        // Перебираємо всі файли в папці
        for (const auto& entry : fs::directory_iterator(path)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                std::string filepath = entry.path().string();

                std::ifstream file(filepath);
                if (!file.is_open()) {
                    std::cerr << "Could not open file: " << filename << std::endl;
                    continue;
                }

                std::string word;
                // Читаємо файл по словах
                while (file >> word) {
                    std::string processedWord = cleanWord(word);
                    if (!processedWord.empty()) {
                        idx.add(processedWord, filename);
                    }
                }
                std::cout << "Indexed: " << filename << std::endl;
            }
        }
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
        std::cerr << "Make sure the 'data' folder exists in the project directory!" << std::endl;
        return 1;
    }

    std::cout << "Indexing complete!\n";
    std::cout << "---------------------------------\n";

    // 3. Інтерактивний пошук
    while (true) {
        std::cout << "\nEnter word to search (or 'exit' to quit): ";
        std::string query;
        std::cin >> query;

        if (query == "exit") break;

        // Очищаємо запит так само як і слова в файлах
        query = cleanWord(query);

        std::vector<std::string> results = idx.search(query);

        if (results.empty()) {
            std::cout << "Word not found.\n";
        }
        else {
            std::cout << "Found in " << results.size() << " files:\n";
            for (const auto& doc : results) {
                std::cout << "- " << doc << "\n";
            }
        }
    }

    return 0;
}