#include "ThreadPool.h"
#include <stdexcept> // Added for std::runtime_error

ThreadPool::ThreadPool(size_t num_threads) : stop_(false) {
    for (size_t i = 0; i < num_threads; ++i) {
        workers_.emplace_back([this] {
            while (true) {
                std::function<void()> task;

                {
                    // Lock the queue to retrieve a task
                    std::unique_lock<std::mutex> lock(this->queue_mutex_);

                    // Wait until there is a task or the pool is stopped
                    this->condition_.wait(lock, [this] {
                        return this->stop_ || !this->tasks_.empty();
                        });

                    // Exit the thread if stopped and queue is empty
                    if (this->stop_ && this->tasks_.empty()) {
                        return;
                    }

                    // Move the task from the queue to local variable
                    task = std::move(this->tasks_.front());
                    this->tasks_.pop();
                }

                // Execute the task outside the lock to allow parallelism
                task();
            }
            });
    }
}

ThreadPool::~ThreadPool() {
    {
        // Lock to set the stop flag safely
        std::unique_lock<std::mutex> lock(queue_mutex_);
        stop_ = true;
    }

    // Wake up all threads so they can finish and exit
    condition_.notify_all();

    // Join all threads to ensure clean shutdown
    for (std::thread& worker : workers_) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

void ThreadPool::Enqueue(std::function<void()> task) {
    {
        std::unique_lock<std::mutex> lock(queue_mutex_);

        // Don't allow enqueueing after shutdown
        if (stop_) {
            throw std::runtime_error("Enqueue on stopped ThreadPool");
        }

        tasks_.push(task);
    }
    // Wake up one worker thread to handle the new task
    condition_.notify_one();
}

size_t ThreadPool::GetQueueSize() {
    std::unique_lock<std::mutex> lock(queue_mutex_);
    return tasks_.size();
}