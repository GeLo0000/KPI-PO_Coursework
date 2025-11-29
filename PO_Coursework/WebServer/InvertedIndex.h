// InvertedIndex.h
#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>

class InvertedIndex {
private:
    // Основна структура: Слово -> Набір назв файлів
    std::map<std::string, std::set<std::string>> index;

public:
    // Метод додавання слова в індекс
    void add(const std::string& word, const std::string& filename) {
        index[word].insert(filename);
    }

    // Метод пошуку
    std::vector<std::string> search(const std::string& word) {
        std::vector<std::string> result;

        // Шукаємо слово в мапі
        auto it = index.find(word);

        // Якщо знайшли
        if (it != index.end()) {
            // Копіюємо з set у vector для результату
            for (const auto& file : it->second) {
                result.push_back(file);
            }
        }

        return result;
    }
};