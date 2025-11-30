#pragma once

#include <string>
#include <vector>
#include <set>
#include <thread>
#include <atomic>
#include "InvertedIndex.h"
#include "ThreadPool.h"

/// @brief background service that monitors a directory and indexes new files.
class Indexer {
public:
    /// @brief Constructs the Indexer.
    /// @param idx Reference to the InvertedIndex to populate.
    /// @param tp Reference to the ThreadPool for executing tasks.
    /// @param path Path to the directory to monitor.
    Indexer(InvertedIndex& idx, ThreadPool& tp, const std::string& path);

    /// @brief Destructor. Stops the background thread if running.
    ~Indexer();

    /// @brief Starts the background monitoring thread.
    /// Non-blocking.
    void Start();

    /// @brief Stops the background monitoring thread.
    void Stop();

private:
    /// @brief The main loop running in the background thread.
    /// Checks for new files periodically.
    void IndexingLoop();

    /// @brief Reads and parses a single file, adding words to the index.
    /// @param filepath Full path to the file.
    /// @param filename Name of the file (used as ID in index).
    void ProcessFile(const std::string& filepath, const std::string& filename);

    // Dependencies
    InvertedIndex& index_;
    ThreadPool& pool_;
    std::string directory_path_;

    // Set of files already submitted for indexing
    std::set<std::string> seen_files_;

    // Thread management
    std::thread scheduler_thread_;
    std::atomic<bool> running_;
};