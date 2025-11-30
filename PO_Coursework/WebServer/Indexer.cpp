#include "Indexer.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <chrono>

namespace fs = std::filesystem;

Indexer::Indexer(InvertedIndex& idx, ThreadPool& tp, const std::string& path)
    : index_(idx), pool_(tp), directory_path_(path), running_(false) {
}

Indexer::~Indexer() {
    Stop();
}

void Indexer::Start() {
    if (running_) {
        return;
    }
    running_ = true;

    // Start the background loop in a separate thread
    scheduler_thread_ = std::thread(&Indexer::IndexingLoop, this);

    std::cout << "[SCHEDULER] Started monitoring directory: " << directory_path_ << "\n";
}

void Indexer::Stop() {
    running_ = false;
    if (scheduler_thread_.joinable()) {
        scheduler_thread_.join();
    }
}

void Indexer::IndexingLoop() {
    while (running_) {
        try {
            bool found_new = false;

            // Iterate over the directory
            for (const auto& entry : fs::directory_iterator(directory_path_)) {
                if (entry.is_regular_file()) {
                    std::string filename = entry.path().filename().string();

                    // Check if we have already seen this file
                    if (seen_files_.count(filename) == 0) {
                        // New file found!
                        seen_files_.insert(filename);

                        // Submit task to thread pool
                        std::string path = entry.path().string();

                        // Note: We use Enqueue instead of enqueue
                        pool_.Enqueue([this, path, filename]() {
                            this->ProcessFile(path, filename);
                            });

                        std::cout << "[SCHEDULER] Found new file: " << filename << "\n";
                        found_new = true;
                    }
                }
            }

            if (found_new) {
                std::cout << "[SCHEDULER] New tasks submitted to ThreadPool.\n";
            }

        }
        catch (const std::exception& e) {
            std::cerr << "Indexer Error: " << e.what() << std::endl;
        }

        // Sleep for 5 seconds, checking running_ flag frequently for quick shutdown
        for (int i = 0; i < 50; ++i) {
            if (!running_) break;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}

void Indexer::ProcessFile(const std::string& filepath, const std::string& filename) {
    std::ifstream file(filepath);
    if (!file.is_open()) return;

    std::string word;
    while (file >> word) {
        // Use static method CleanWord (PascalCase)
        std::string processed = InvertedIndex::CleanWord(word);
        if (!processed.empty()) {
            // Use Add (PascalCase)
            index_.Add(processed, filename);
        }
    }
    // Update stats
    index_.IncrementFileCount();
}