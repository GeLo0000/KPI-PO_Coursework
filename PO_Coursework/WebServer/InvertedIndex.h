// InvertedIndex.h
#pragma once
#include <string>
#include <vector>
#include <map>
#include <set>
#include <shared_mutex>
#include <algorithm>

class InvertedIndex {
private:
    std::map<std::string, std::set<std::string>> index;
    mutable std::shared_mutex mtx;

public:
    void add(const std::string& word, const std::string& filename);
    std::vector<std::string> search(const std::string& word) const;
    size_t getSize() const;

    // Статичний метод-помічник для очистки слів (щоб був доступний всім)
    static std::string cleanWord(const std::string& word);
};