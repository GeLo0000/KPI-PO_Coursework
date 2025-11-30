#include "InvertedIndex.h"
#include <functional> // For std::hash
#include <cctype>     // For isalnum, tolower

InvertedIndex::InvertedIndex() {
    // Initialize independent shards
    for (int i = 0; i < kNumShards; ++i) {
        shards_.push_back(std::make_unique<IndexShard>());
    }
}

size_t InvertedIndex::GetShardIndex(const std::string& word) const {
    std::hash<std::string> hasher;
    return hasher(word) % kNumShards;
}

void InvertedIndex::Add(const std::string& word, const std::string& filename) {
    size_t shard_idx = GetShardIndex(word);
    IndexShard* shard = shards_[shard_idx].get();

    // Exclusive lock for writing (blocks readers and writers for THIS shard)
    std::unique_lock<std::shared_mutex> lock(shard->mtx);
    shard->data[word].insert(filename);
}

std::vector<std::string> InvertedIndex::Search(const std::string& word) const {
    std::vector<std::string> result;
    size_t shard_idx = GetShardIndex(word);
    IndexShard* shard = shards_[shard_idx].get();

    // Shared lock for reading (allows other readers)
    std::shared_lock<std::shared_mutex> lock(shard->mtx);

    auto it = shard->data.find(word);
    if (it != shard->data.end()) {
        for (const auto& file : it->second) {
            result.push_back(file);
        }
    }
    return result;
}

int InvertedIndex::GetFilesCount() const {
    return docs_count_.load();
}

void InvertedIndex::IncrementFileCount() {
    docs_count_++;
}

std::string InvertedIndex::CleanWord(const std::string& word) {
    std::string clean;
    for (char c : word) {
        if (std::isalnum(static_cast<unsigned char>(c))) {
            clean += std::tolower(static_cast<unsigned char>(c));
        }
    }
    return clean;
}