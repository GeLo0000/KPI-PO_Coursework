#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>
#include <shared_mutex>
#include <memory>
#include <atomic>

/// @brief Constant defining the number of segments for the index (sharding).
/// Helps to reduce lock contention during concurrent writes.
constexpr int kNumShards = 32;

/// @brief Represents a single segment of the index with its own lock.
struct IndexShard {
    std::map<std::string, std::set<std::string>> data;
    mutable std::shared_mutex mtx;
};

/// @brief Thread-safe Inverted Index structure.
/// Stores mapping from words to documents using sharding for performance.
class InvertedIndex {
public:
    InvertedIndex();

    /// @brief Adds a word and its corresponding filename to the index.
    /// Thread-safe: locks only one specific shard.
    /// @param word The word to index.
    /// @param filename The name of the file containing the word.
    void Add(const std::string& word, const std::string& filename);

    /// @brief Searches for a word in the index.
    /// Thread-safe: uses shared lock (multiple readers allowed).
    /// @param word The word to search for.
    /// @return A list of filenames where the word appears.
    std::vector<std::string> Search(const std::string& word) const;

    /// @brief Returns the total number of fully processed files.
    /// @return Count of indexed files.
    int GetFilesCount() const;

    /// @brief Atomically increments the processed file counter.
    /// Should be called once per file after it is fully indexed.
    void IncrementFileCount();

    /// @brief Static helper to clean a word (lowercase, remove punctuation).
    /// @param word Raw word.
    /// @return Cleaned word.
    static std::string CleanWord(const std::string& word);

private:
    /// @brief Determines which shard a word belongs to based on hashing.
    size_t GetShardIndex(const std::string& word) const;

    // Vector of pointers to independent shards
    std::vector<std::unique_ptr<IndexShard>> shards_;

    // Atomic counter for processed files (no mutex needed)
    std::atomic<int> docs_count_{ 0 };
};