// main.cpp
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <vector>
#include <chrono> // Для затримок (емуляція)

#include "InvertedIndex.h"
#include "ThreadPool.h"

namespace fs = std::filesystem;

// Функція очистки слова (залишаємо як було)
std::string cleanWord(const std::string& word) {
    std::string clean;
    for (char c : word) {
        if (isalnum((unsigned char)c)) {
            clean += tolower((unsigned char)c);
        }
    }
    return clean;
}

// Функція, яку виконуватиме потік: читання ОДНОГО файлу
void processFile(const std::string& filepath, const std::string& filename, InvertedIndex& idx) {
    std::ifstream file(filepath);
    if (!file.is_open()) return;

    std::string word;
    while (file >> word) {
        std::string processed = cleanWord(word);
        if (!processed.empty()) {
            // Тут викликається потокобезпечний метод add
            idx.add(processed, filename);
        }
    }
    // Для наочності можна розкоментувати, але буде багато спаму в консоль
    // std::cout << "Thread " << std::this_thread::get_id() << " indexed: " << filename << "\n";
}

int main() {
    // 1. Налаштування
    std::string directoryPath = "data";
    int threadCount = 4; // Можна змінювати
    int batchSize = 50;  // Розмір пакету файлів

    InvertedIndex idx;
    ThreadPool pool(threadCount);

    std::cout << "Starting Indexing System with " << threadCount << " threads...\n";

    // 2. Збираємо всі шляхи до файлів у вектор
    std::vector<std::pair<std::string, std::string>> allFiles; // <path, filename>
    try {
        for (const auto& entry : fs::directory_iterator(directoryPath)) {
            if (entry.is_regular_file()) {
                allFiles.push_back({ entry.path().string(), entry.path().filename().string() });
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error reading directory: " << e.what() << "\n";
        return 1;
    }

    std::cout << "Found " << allFiles.size() << " files to index.\n";

    // 3. Запускаємо "Клієнта" в окремому потоці (емуляція запитів під час індексації)
    bool indexingFinished = false;
    std::thread clientThread([&idx, &indexingFinished]() {
        std::string queries[] = { "fox", "dog", "internet", "c++", "java" };
        int i = 0;
        while (!indexingFinished) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Запит кожні 0.5 сек
            std::string q = queries[i % 5];

            // Потокобезпечний пошук
            auto results = idx.search(q);

            // Виводимо результат (обережно з cout у потоках, може змішатись, але для демо ок)
            // Використовуємо printf, бо він рідше "рве" рядки
            printf("[CLIENT] Search '%s' -> found in %llu docs. (Index size: %llu keys)\n",
                q.c_str(), results.size(), idx.getSize());
            i++;
        }
        std::cout << "[CLIENT] Finished searching session.\n";
        });

    // 4. Пакетна обробка файлів
    for (size_t i = 0; i < allFiles.size(); i += batchSize) {
        std::cout << "--- Loading batch " << (i / batchSize) + 1 << " (" << batchSize << " files) ---\n";

        // Формуємо пакет завдань
        size_t end = std::min(i + batchSize, allFiles.size());
        for (size_t j = i; j < end; ++j) {
            // Додаємо задачу в пул
            // std::bind або лямбда, захоплюємо посилання на idx і копії рядків
            pool.enqueue([path = allFiles[j].first, name = allFiles[j].second, &idx]() {
                processFile(path, name, idx);
                });
        }

        // Емуляція затримки між завантаженнями пакетів (щоб побачити динаміку)
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    // Даємо час потокам допрацювати останні задачі
    std::cout << "All batches loaded. Waiting for completion...\n";
    std::this_thread::sleep_for(std::chrono::seconds(2)); // Проста пауза, щоб пул допрацював

    // Зупиняємо клієнта
    indexingFinished = true;
    if (clientThread.joinable()) clientThread.join();

    std::cout << "Indexing finished completely!\n";

    // Тут Пул потоків автоматично знищиться (деструктор), дочекавшись завершення потоків

    // 5. Фінальна перевірка вручну
    while (true) {
        std::cout << "\nFinal check > Enter word (or 'exit'): ";
        std::string q;
        std::cin >> q;
        if (q == "exit") break;

        auto res = idx.search(cleanWord(q));
        std::cout << "Found in " << res.size() << " files.\n";
    }

    return 0;
}