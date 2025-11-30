// main.cpp
#include <iostream>
#include <string>
#include "InvertedIndex.h"
#include "ThreadPool.h"
#include "Indexer.h"
#include "WebServer.h"

int main() {
    // 1. Створюємо інвертований індекс (спільний ресурс)
    InvertedIndex index;

    // 2. Створюємо ДВА окремі пули потоків

    // Пул для індексації: нехай працює на 4 ядрах (важка робота)
    ThreadPool indexingPool(20);

    // Пул для клієнтів: вистачить 2-х потоків (вони працюють швидко)
    // Це гарантує, що для клієнтів ЗАВЖДИ є вільні руки, 
    // незалежно від того, скільки файлів ми зараз читаємо.
    ThreadPool clientPool(2);

    std::cout << "System initialized:\n";
    std::cout << "- Indexing Pool: 4 threads\n";
    std::cout << "- Client Pool:   2 threads\n";

    // 3. Запуск індексації (використовує indexingPool)
    // Indexer швидко закине задачі в indexingPool і поверне керування
    Indexer indexer(index, indexingPool, "data");
    indexer.run();

    // 4. Запуск сервера (використовує clientPool)
    // Server буде кидати нових клієнтів у clientPool, який вільний!
    // Передаємо indexingPool четвертим аргументом
    WebServer server(8080, index, clientPool, indexingPool);
    server.start();

    // Цей метод запускає listen і цикл accept, але accept блокуючий,
    // тому server.start() у нас зараз зроблений так, що він блокує main.
    // АЛЕ! У нас у WebServer.cpp метод start() містить while(isRunning).
    // Це означає, що програма "зависне" всередині server.start().
    // Це нормально, адже indexer вже працює у фоні (в indexingPool).
    server.start();

    return 0;
}