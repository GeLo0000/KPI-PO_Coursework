#pragma once

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>

/// @brief Manages a fixed number of worker threads to execute tasks concurrently.
class ThreadPool {
public:
    /// @brief Constructs the thread pool and spins up worker threads.
    /// @param num_threads The number of worker threads to create.
    explicit ThreadPool(size_t num_threads);

    /// @brief Destructor. Stops all threads and joins them.
    ~ThreadPool();

    /// @brief Adds a new void() task to the queue.
    /// @param task The function to execute.
    void Enqueue(std::function<void()> task);

    /// @brief Returns the current number of pending tasks.
    /// @return Number of tasks in the queue.
    size_t GetQueueSize();

private:
    // Worker threads that process the tasks
    std::vector<std::thread> workers_;

    // Queue of tasks waiting to be executed
    std::queue<std::function<void()>> tasks_;

    // Synchronization primitives
    std::mutex queue_mutex_;
    std::condition_variable condition_;

    // Atomic flag to signal threads to stop
    std::atomic<bool> stop_;
};