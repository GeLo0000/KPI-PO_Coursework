#pragma once
// ThreadPool.h
#pragma once
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>

class ThreadPool {
private:
    // Вектор потоків-робітників
    std::vector<std::thread> workers;
    // Черга завдань (функцій void())
    std::queue<std::function<void()>> tasks;

    // Синхронізація для черги
    std::mutex queue_mutex;
    std::condition_variable condition;

    // Прапор зупинки
    std::atomic<bool> stop;

public:
    // Конструктор: запускає threads_count потоків
    ThreadPool(size_t threads_count) : stop(false) {
        for (size_t i = 0; i < threads_count; ++i) {
            workers.emplace_back([this] {
                while (true) {
                    std::function<void()> task;

                    {
                        // Блокуємо чергу, щоб взяти задачу
                        std::unique_lock<std::mutex> lock(this->queue_mutex);

                        // Чекаємо, поки з'явиться задача або надійде сигнал зупинки
                        this->condition.wait(lock, [this] {
                            return this->stop || !this->tasks.empty();
                            });

                        // Якщо зупинка і задач немає - виходимо
                        if (this->stop && this->tasks.empty())
                            return;

                        // Беремо задачу
                        task = std::move(this->tasks.front());
                        this->tasks.pop();
                    }

                    // Виконуємо задачу (вже без блокування черги!)
                    task();
                }
                });
        }
    }

    // Додавання задачі в чергу
    void enqueue(std::function<void()> task) {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            // Не дозволяємо додавати задачі, якщо пул зупинено
            if (stop) throw std::runtime_error("enqueue on stopped ThreadPool");
            tasks.push(task);
        }
        // Будимо один сплячий потік
        condition.notify_one();
    }

    // Деструктор: коректно завершує всі потоки
    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            stop = true;
        }
        condition.notify_all(); // Будимо всіх, щоб вони завершились
        for (std::thread& worker : workers) {
            if (worker.joinable())
                worker.join();
        }
    }
};