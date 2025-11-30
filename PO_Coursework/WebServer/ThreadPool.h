#pragma once

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>

// Manages a fixed number of worker threads to execute tasks concurrently.
class ThreadPool {
public:
    // Constructs the thread pool and spins up worker threads.
    explicit ThreadPool(size_t num_threads);

    // Destructor. Stops all threads and joins them.
    ~ThreadPool();

    // Adds a new void() task to the queue.
    void Enqueue(std::function<void()> task);

    // Returns the current number of pending tasks.
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