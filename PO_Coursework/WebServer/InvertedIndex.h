// InvertedIndex.h
#pragma once
#include <string>
#include <vector>
#include <map>
#include <set>
#include <shared_mutex>
#include <memory>
#include <atomic> // <--- Підключаємо атоміки

const int NUM_SHARDS = 32;

struct IndexShard {
    std::map<std::string, std::set<std::string>> data;
    mutable std::shared_mutex mtx;
};

class InvertedIndex {
private:
    std::vector<std::unique_ptr<IndexShard>> shards;

    // НОВЕ: Замість std::set використовуємо атомарний лічильник
    std::atomic<int> docsCount{ 0 };

    size_t getShardIndex(const std::string& word) const;

public:
    InvertedIndex();

    void add(const std::string& word, const std::string& filename);
    std::vector<std::string> search(const std::string& word) const;

    // Оновлені методи для статистики
    int getFilesCount() const;      // Просто повертає число
    void incrementFileCount();      // Додає +1 (викликатимемо, коли файл готовий)

    static std::string cleanWord(const std::string& word);
};