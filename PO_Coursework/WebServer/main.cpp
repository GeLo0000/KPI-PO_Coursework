// main.cpp
#include <iostream>
#include "InvertedIndex.h"
#include "ThreadPool.h"
#include "Indexer.h"
#include "WebServer.h"

int main() {
    // 1. Ініціалізація компонентів
    InvertedIndex index;
    ThreadPool pool(4); // 4 потоки

    // 2. Запуск індексації (у фоні, через пул потоків)
    Indexer indexer(index, pool, "data");
    indexer.run();

    // 3. Запуск сервера
    WebServer server(8080, index, pool);
    server.start();

    // Щоб консоль не закрилась одразу (бо сервер у нас поки "порожній" і вийде з методу start)
    std::cout << "Press Enter to exit...";
    std::cin.get();

    return 0;
}