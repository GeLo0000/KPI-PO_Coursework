// InvertedIndex.h
#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <shared_mutex> // C++17: Для read/write locks
#include <mutex>

class InvertedIndex {
private:
    std::map<std::string, std::set<std::string>> index;
    mutable std::shared_mutex mtx; // М'ютекс для захисту мапи

public:
    // Метод додавання (ПИСЬМЕННИК - ексклюзивний доступ)
    void add(const std::string& word, const std::string& filename) {
        // unique_lock блокує все: ніхто не може ні читати, ні писати, поки ми тут
        std::unique_lock<std::shared_mutex> lock(mtx);
        index[word].insert(filename);
    }

    // Метод пошуку (ЧИТАЧ - спільний доступ)
    std::vector<std::string> search(const std::string& word) const {
        // shared_lock дозволяє іншим потокам теж робити search одночасно,
        // але забороняє комусь робити add
        std::shared_lock<std::shared_mutex> lock(mtx);

        std::vector<std::string> result;
        auto it = index.find(word);
        if (it != index.end()) {
            for (const auto& file : it->second) {
                result.push_back(file);
            }
        }
        return result;
    }

    // Отримати розмір індексу (для статистики)
    size_t getSize() const {
        std::shared_lock<std::shared_mutex> lock(mtx);
        return index.size();
    }
};