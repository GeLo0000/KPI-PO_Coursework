// InvertedIndex.cpp
#include "InvertedIndex.h"
#include <functional>

InvertedIndex::InvertedIndex() {
    for (int i = 0; i < NUM_SHARDS; ++i) {
        shards.push_back(std::make_unique<IndexShard>());
    }
}

size_t InvertedIndex::getShardIndex(const std::string& word) const {
    std::hash<std::string> hasher;
    return hasher(word) % NUM_SHARDS;
}

void InvertedIndex::add(const std::string& word, const std::string& filename) {
    size_t shardIdx = getShardIndex(word);
    IndexShard* shard = shards[shardIdx].get();

    std::unique_lock<std::shared_mutex> lock(shard->mtx);
    shard->data[word].insert(filename);

    // ПРИБРАЛИ: тут більше немає коду додавання в processedFiles
}

std::vector<std::string> InvertedIndex::search(const std::string& word) const {
    std::vector<std::string> result;
    size_t shardIdx = getShardIndex(word);
    IndexShard* shard = shards[shardIdx].get();

    std::shared_lock<std::shared_mutex> lock(shard->mtx);
    auto it = shard->data.find(word);
    if (it != shard->data.end()) {
        for (const auto& file : it->second) {
            result.push_back(file);
        }
    }
    return result;
}

// НОВЕ: Просто читаємо атомарну змінну
int InvertedIndex::getFilesCount() const {
    return docsCount.load();
}

// НОВЕ: Збільшуємо лічильник
void InvertedIndex::incrementFileCount() {
    docsCount++;
}

std::string InvertedIndex::cleanWord(const std::string& word) {
    // ... (без змін) ...
    std::string clean;
    for (char c : word) {
        if (isalnum((unsigned char)c)) {
            clean += tolower((unsigned char)c);
        }
    }
    return clean;
}