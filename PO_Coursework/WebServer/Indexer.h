#pragma once

#include <string>
#include <vector>
#include <set>
#include <thread>
#include <atomic>
#include "InvertedIndex.h"
#include "ThreadPool.h"

// Background service that monitors a directory and indexes new files.
class Indexer {
public:
    // Constructs the Indexer.
    Indexer(InvertedIndex& idx, ThreadPool& tp, const std::string& path);

    // Destructor. Stops the background thread if running.
    ~Indexer();

    // Starts the background monitoring thread.
    void Start();

    // Stops the background monitoring thread.
    void Stop();

private:
    // The main loop running in the background thread.
    // Checks for new files periodically.
    void IndexingLoop();

    // Reads and parses a single file, adding words to the index.
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